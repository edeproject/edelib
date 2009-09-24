/*
 * $Id$
 *
 * D-BUS stuff
 * Copyright (c) 2008 edelib authors
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <string.h>

#include <FL/Fl.H>
#include <dbus/dbus.h>

#include <edelib/List.h>
#include <edelib/EdbusConnection.h>
#include <edelib/EdbusObjectPath.h>

/* newer dbus versions deprecate dbus_watch_get_fd */
#ifdef HAVE_DBUS_WATCH_GET_UNIX_FD
# define dbus_watch_get_fd dbus_watch_get_unix_fd
#endif

EDELIB_NS_BEGIN

typedef list<DBusWatch*> WatchList;
typedef list<DBusWatch*>::iterator WatchListIter;

typedef list<const char*> ObjectPathList;
typedef list<const char*>::iterator ObjectPathListIter;

struct EdbusConnImpl {
	DBusConnection* conn;

	EdbusCallback   signal_cb;
	void*           signal_cb_data;

	EdbusCallback   method_call_cb;
	void*           method_call_cb_data;

	ObjectPathList object_list;

	/* 
	 * only used when setup_listener_with_fltk() is called
	 * TODO: can WatchList be replaced with DBusWatchList ?
	 */
	WatchList*      watch_list;
	DBusTimeout*    timeout;

	/* signal and method tables */
	EdbusCallbackItem* signal_table;
	unsigned int       signal_table_sz;

	EdbusCallbackItem* method_table;
	unsigned int       method_table_sz;

	/*
	 * so we can know how many times 
	 * add_signal_match()/add_method_match() was called 
	 */
	unsigned int       signal_matches;
	unsigned int       method_matches;
};

static EdbusCallbackItem* scan_callback_table(EdbusCallbackItem* table, unsigned int sz, const EdbusMessage& msg) {
	E_ASSERT(table != NULL);

	for(unsigned int i = 0; i < sz; i++) {
		if((strcmp(table[i].interface, msg.interface()) == 0) && (strcmp(table[i].name, msg.member()) == 0)) {
			/* allow NULL in path */
			if(!table[i].path)
				return &table[i];

			/* have path, scan it */
			if(strcmp(table[i].path, msg.path()) == 0)
				return &table[i];
		}
	}

	return NULL;
}

static bool have_registered_object(EdbusConnImpl* dc, const char* path) {
	if(dc->object_list.empty())
		return true;

	ObjectPathListIter it = dc->object_list.begin(), it_end = dc->object_list.end();
	for(; it != it_end; ++it) {
		if(strcmp(*it, path) == 0)
			return true;
	}

	return false;
}

static DBusHandlerResult edbus_signal_filter(DBusConnection* connection, DBusMessage* msg, void* data) {
	E_ASSERT(data != NULL);
	E_ASSERT(msg != NULL);

	EdbusConnImpl* dc = (EdbusConnImpl*)data;
	int mtype = dbus_message_get_type(msg);
	int ret = 0;

	/* 
	 * Check first if service set some objects before we do further.
	 *
	 * Notice that if we do not have any registered object, this will 
	 * be seen as true. Allowing this will allow us to track some services
	 * that emits a lot of signals, like HAL
	 */
	if(!have_registered_object(dc, dbus_message_get_path(msg)))
		goto out;

	if(mtype == DBUS_MESSAGE_TYPE_SIGNAL) {
		if(dc->signal_table || dc->signal_cb) {
			EdbusMessage m(msg);

			if(dc->signal_table) {
				/* signal table has precedence over plain signal callback */
				EdbusCallbackItem* item = scan_callback_table(dc->signal_table, dc->signal_table_sz, m);
				if(item) {
					ret = (item->callback)(&m, item->data);
					goto out;
				}
			} else if(dc->signal_cb) {
				/* call signal callback */
				ret = (dc->signal_cb)(&m, dc->signal_cb_data);
				goto out;
			}
		}
	} 
	
	if(mtype == DBUS_MESSAGE_TYPE_METHOD_CALL) {
		if(dc->method_table || dc->method_call_cb) {
			EdbusMessage m(msg);

			if(dc->method_table) {
				/* method table has precedence over plain method callback */
				EdbusCallbackItem* item = scan_callback_table(dc->method_table, dc->method_table_sz, m);
				if(item) {
					ret = (item->callback)(&m, item->data);
					goto out;
				}
			} else if(dc->method_call_cb) {
				/* call method callback */
				ret = (dc->method_call_cb)(&m, dc->method_call_cb_data);
				goto out;
			}
		}
	}

out:
	return ((ret > 0) ? DBUS_HANDLER_RESULT_HANDLED : DBUS_HANDLER_RESULT_NOT_YET_HANDLED);
}

static void dispatch_cb(void* d) {
	EdbusConnImpl* dc = (EdbusConnImpl*)d;
	E_ASSERT(dc != NULL);

	/* E_DEBUG(E_STRLOC ": dispatch_cb()\n"); */

	while(dbus_connection_dispatch(dc->conn) == DBUS_DISPATCH_DATA_REMAINS)
		;

	Fl::remove_timeout(dispatch_cb);
}

static void read_watch_cb(int fd, void* d) { 
	/* E_DEBUG(E_STRLOC ": read_watch_cb()\n"); */

	EdbusConnImpl* dc = (EdbusConnImpl*)d;
	E_ASSERT(dc != NULL);
	E_ASSERT(dc->watch_list != NULL);

	WatchListIter it = dc->watch_list->begin(), it_end = dc->watch_list->end();
	while(it != it_end) {
		if(dbus_watch_get_fd(*it) == fd && dbus_watch_get_enabled(*it)) {
			if(!dbus_watch_handle(*it, DBUS_WATCH_READABLE))
				E_WARNING(E_STRLOC ": Out of memory\n");
			break;
		}
		++it;
	}

	/*
	 * Check if there are more incomming data and process them. Note that 
	 * dbus_connection_dispatch() call will also remove data from queue. 
	 * This means that (here) timer will not be installed if only one unprocessed
	 * message is in queue; opposite, after installment it will process the rest
	 * of the messages without interrupting read_watch_cb() flow.
	 *
	 * If this is not set (e.g. all data are processed here), we can miss initial
	 * (or later) messages that are sent to us. Also, timer will be triggered faster
	 * as it can (seems that 0.5 as timer value misses some data...).
	 */
	if(dbus_connection_dispatch(dc->conn) == DBUS_DISPATCH_DATA_REMAINS)
		Fl::add_timeout(0.2, dispatch_cb, dc);
}

static void write_watch_cb(int fd, void* d) { 
	/* E_DEBUG(E_STRLOC ": write_watch_cb()\n"); */

	EdbusConnImpl* dc = (EdbusConnImpl*)d;
	E_ASSERT(dc != NULL);
	E_ASSERT(dc->watch_list != NULL);

	WatchListIter it = dc->watch_list->begin(), it_end = dc->watch_list->end();
	while(it != it_end) { 
		if(dbus_watch_get_fd(*it) == fd && dbus_watch_get_enabled(*it)) {
			if(!dbus_watch_handle(*it, DBUS_WATCH_WRITABLE))
				E_WARNING(E_STRLOC ": Out of memory\n");
			break;
		}
		++it;
	}
}

static void timeout_cb(void* d) {
	/* E_DEBUG(E_STRLOC ": timeout_cb()\n"); */

	EdbusConnImpl* dc = (EdbusConnImpl*)d;
	E_ASSERT(dc != NULL);
	E_ASSERT(dc->timeout != NULL);

	dbus_timeout_handle(dc->timeout);
}

static dbus_bool_t edbus_add_watch(DBusWatch* watch, void* d) {
	E_ASSERT(watch != NULL);

	/* 
	 * check if watch is enabled since dbus_watch_handle() can't be used
	 * on as connection will not be ready to handle this watch yet
	 */
 	if(!dbus_watch_get_enabled(watch))
		return 1;

	EdbusConnImpl* dc = (EdbusConnImpl*)d;
	E_ASSERT(dc != NULL);
	E_ASSERT(dc->watch_list != NULL);

	int fd = dbus_watch_get_fd(watch);
	int flags = dbus_watch_get_flags(watch);

	dc->watch_list->push_back(watch);

	if(flags & DBUS_WATCH_READABLE)
		Fl::add_fd(fd, FL_READ, read_watch_cb, d);

	if(flags & DBUS_WATCH_WRITABLE)
		Fl::add_fd(fd, FL_WRITE, write_watch_cb, d);

	return 1;
}

static void edbus_remove_watch(DBusWatch* watch, void* d) {
	E_ASSERT(watch != NULL);

	EdbusConnImpl* dc = (EdbusConnImpl*)d;
	E_ASSERT(dc != NULL);
	E_ASSERT(dc->watch_list != NULL);

	/* E_DEBUG(E_STRLOC ": removing watch\n"); */

	int fd = dbus_watch_get_fd(watch);
	int flags = dbus_watch_get_flags(watch);

	if(flags & DBUS_WATCH_READABLE)
		Fl::remove_fd(fd, FL_READ);

	if(flags & DBUS_WATCH_WRITABLE)
		Fl::remove_fd(fd, FL_WRITE);

	WatchListIter it = dc->watch_list->begin(), it_end = dc->watch_list->end();
	while(it != it_end) {
		if(*it == watch) {
			dc->watch_list->erase(it);
			break;
		}
		++it;
	}
}

static void edbus_toggle_watch(DBusWatch* watch, void* data) {
	E_ASSERT(watch != NULL);

	if(dbus_watch_get_enabled(watch))
		edbus_add_watch(watch, data);
	else
		edbus_remove_watch(watch, data);
}

static dbus_bool_t edbus_add_timeout(DBusTimeout* timeout, void* data) {
	E_ASSERT(timeout != NULL);

	/* D-Bus interval sees in miliseconds, but FLTK see it in seconds */
	int interval = dbus_timeout_get_interval(timeout);

	/* E_DEBUG(E_STRLOC ": added timeout to %i ms\n", interval); */
	Fl::add_timeout(interval / 1000, timeout_cb, data);
	return 1;
}

static void edbus_remove_timeout(DBusTimeout* timeout, void* data) {
	E_ASSERT(timeout != NULL);
	Fl::remove_timeout(timeout_cb);
}

static void edbus_toggle_timeout(DBusTimeout* timeout, void* data) {
	E_ASSERT(timeout != NULL);

	if(dbus_timeout_get_enabled(timeout))
		edbus_add_timeout(timeout, data);
	else
		edbus_remove_timeout(timeout, data);
}

static void edbus_wakeup_main(void*) {
	/* Not used */
}


EdbusConnection::EdbusConnection() : dc(NULL) {
}

EdbusConnection::~EdbusConnection() {
	disconnect();
	delete dc;
}

void EdbusConnection::setup_filter(void) {
	E_ASSERT(dc != NULL);
	E_ASSERT(dc->conn != NULL);

	DBusError err;
	dbus_error_init(&err);

	/* if we didn't registered any signal match, match everything */
	if(dc->signal_matches == 0) {
		dbus_bus_add_match(dc->conn, "type='signal'", &err);

		if(dbus_error_is_set(&err)) {
			E_WARNING(E_STRLOC ": Signal match failed: %s, %s\n", err.name, err.message);
			dbus_error_free(&err);
			return;
		}
	}

	/* if we didn't registered any method match, match everything */
	if(dc->method_matches == 0) {
		const char* name = unique_name();
		if(name) {
			char buff[DBUS_MAXIMUM_MATCH_RULE_LENGTH];
			snprintf(buff, sizeof(buff), "destination='%s'", name);

			dbus_bus_add_match(dc->conn, buff, &err);

			if(dbus_error_is_set(&err)) {
				E_WARNING(E_STRLOC ": Destination match failed: %s, %s\n", err.name, err.message);
				dbus_error_free(&err);
				return;
			}
		} else
			E_WARNING(E_STRLOC ": Unable to get unique name\n");
	}

	dbus_connection_add_filter(dc->conn, edbus_signal_filter, dc, 0);
}

bool EdbusConnection::connect(EdbusConnectionType ctype) {
	if(dc == NULL) {
		dc = new EdbusConnImpl;
		dc->conn = NULL;

		dc->signal_cb = NULL;
		dc->signal_cb_data = NULL;

		dc->method_call_cb = NULL;
		dc->method_call_cb_data = NULL;

		dc->watch_list = NULL;
		dc->timeout = NULL;

		dc->signal_table = NULL;
		dc->signal_table_sz = 0;

		dc->method_table = NULL;
		dc->method_table_sz = 0;

		dc->signal_matches = dc->method_matches = 0;
	}

	DBusBusType type;
	DBusError err;
	dbus_error_init(&err);

	switch(ctype) {
		case EDBUS_SYSTEM:
			type = DBUS_BUS_SYSTEM;
			break;
		case EDBUS_SESSION:
		default:
			type = DBUS_BUS_SESSION;
			break;
	}

	dc->conn = dbus_bus_get(type, &err);

	if(dbus_error_is_set(&err)) {
		E_WARNING(E_STRLOC ": Connection error: %s\n", err.message);
		dbus_error_free(&err);
	}

	if(dc->conn == NULL)
		return false;

	return true;
}

bool EdbusConnection::disconnect(void) {
	/* only non-shared connections are allowed to be closed */
	if(dc->conn)
		dbus_connection_unref(dc->conn);

	dc->conn = NULL;

	/* TODO: does this needs to be nulled ? */
	dc->signal_cb = NULL;
	dc->signal_cb_data = NULL;

	dc->method_call_cb = NULL;
	dc->method_call_cb_data = NULL;

	dc->object_list.clear();

	dc->signal_table = NULL;
	dc->signal_table_sz = 0;

	dc->method_table = NULL;
	dc->method_table_sz = 0;

	dc->signal_matches = dc->method_matches = 0;

	/* remove all FLTK notifiers so we can reuse EdbusConnection object again */
	if(dc->watch_list) {
		int fd;
		WatchListIter it = dc->watch_list->begin(), it_end = dc->watch_list->end();

		while(it != it_end) {
			fd = dbus_watch_get_fd(*it);
			Fl::remove_fd(fd);
			++it;
		}

		dc->watch_list->clear();
		delete dc->watch_list;
		dc->watch_list = NULL;
	}

	if(dc->timeout) {
		Fl::remove_timeout(timeout_cb);
		dc->timeout = NULL;
	}

	return true;
}

bool EdbusConnection::send(const EdbusMessage& content) {
	if(!dc || !dc->conn)
		return false;

	bool ret;
	dbus_uint32_t serial;

	DBusMessage* msg = content.to_dbus_message();
	if(!msg) {
		E_WARNING(E_STRLOC ": Can't convert to DBusMessage\n");
		return false;
	}

	if(!dbus_connection_send(dc->conn, msg, &serial)) {
		E_WARNING(E_STRLOC ": Message sending failed\n");
		ret = false;
	} else
		ret = true;

	dbus_connection_flush(dc->conn);
	return ret;
}

bool EdbusConnection::send_with_reply_and_block(const EdbusMessage& content, int timeout_ms, EdbusMessage& ret) {
	if(!dc || !dc->conn)
		return false;

	DBusMessage* reply, *msg;
	DBusError err;

	dbus_error_init(&err);

	msg = content.to_dbus_message();
	if(!msg) {
		E_WARNING(E_STRLOC ": Can't convert to DBusMessage\n");
		return false;
	}

	reply = dbus_connection_send_with_reply_and_block(dc->conn, msg, timeout_ms, &err);

	if(dbus_error_is_set(&err)) {
		E_WARNING(E_STRLOC ": Sending error: %s, %s\n", err.name, err.message);
		dbus_error_free(&err);
		return false;
	}

	ret.from_dbus_message(reply);
	return true;
}

bool EdbusConnection::request_name(const char* name, int mode) {
	if(!dc || !dc->conn)
		return false;

	E_ASSERT(mode >= 0 && "Invalid 'mode' value");

	int flags = 0;
	bool ret = false;

	/* mixing them have no much sense */
	if(mode & EDBUS_NAME_NO_REPLACE)
		flags = 0;
	else {
		if(mode & EDBUS_NAME_ALLOW_REPLACE)
			flags |= DBUS_NAME_FLAG_ALLOW_REPLACEMENT;

		if(mode & EDBUS_NAME_REPLACE_EXISTING)
			flags |= DBUS_NAME_FLAG_REPLACE_EXISTING;
	}

	DBusError err;
	dbus_error_init(&err);

	/* see if we got the name */
	if(dbus_bus_request_name(dc->conn, name, flags, &err) == DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
		ret = true;
	else
		ret = false;

	if(dbus_error_is_set(&err)) {
		E_WARNING(E_STRLOC ": Name request error: %s, %s\n", err.name, err.message);
		dbus_error_free(&err);
		ret = false;
	}

	return ret;
}

const char* EdbusConnection::unique_name(void) {
	if(!dc || !dc->conn)
		return NULL;
	return dbus_bus_get_unique_name(dc->conn);
}

void EdbusConnection::signal_callback(EdbusCallback cb, void* data) {
	if(!dc)
		return;

	dc->signal_cb = cb;
	dc->signal_cb_data = data;
}

void EdbusConnection::method_callback(EdbusCallback cb, void* data) {
	if(!dc)
		return;

	dc->method_call_cb = cb;
	dc->method_call_cb_data = data;
}

void EdbusConnection::signal_callback_table(EdbusCallbackItem* table, unsigned int sz) {
	if(!dc)
		return;

	dc->signal_table = table;
	dc->signal_table_sz = sz;
}

void EdbusConnection::signal_callback_table_data(unsigned int pos, void* data) {
	if(!dc)
		return;

	E_ASSERT(dc->signal_table != NULL && "You must first set table via signal_callback_table()");
	E_ASSERT(dc->signal_table_sz > pos && "Table member out of bounds");

	dc->signal_table[pos].data = data;
}

void EdbusConnection::method_callback_table(EdbusCallbackItem* table, unsigned int sz) {
	if(!dc)
		return;

	dc->method_table = table;
	dc->method_table_sz = sz;
}

void EdbusConnection::method_callback_table_data(unsigned int pos, void* data) {
	if(!dc)
		return;

	E_ASSERT(dc->method_table != NULL && "You must first set table via method_callback_table()");
	E_ASSERT(dc->method_table_sz > pos && "Table member out of bounds");

	dc->method_table[pos].data = data;
}

void EdbusConnection::add_signal_match(const char* path, const char* interface, const char* name) {
	if(!dc || !dc->conn)
		return;

	DBusError err;
	dbus_error_init(&err);

	char buff[DBUS_MAXIMUM_MATCH_RULE_LENGTH];
	snprintf(buff, sizeof(buff), "type='signal',path='%s',interface='%s',member='%s'", path, interface, name);

	dbus_bus_add_match(dc->conn, buff, &err);
	if(dbus_error_is_set(&err)) {
		E_WARNING(E_STRLOC ": Adding signal match failed: %s, %s\n", err.name, err.message);
		dbus_error_free(&err);
		return;
	}

	dc->signal_matches++;
}

void EdbusConnection::add_method_match(const char* path, const char* interface, const char* name) {
	if(!dc || !dc->conn)
		return;

	const char* u = unique_name();
	if(!u)
		return;

	DBusError err;
	dbus_error_init(&err);

	char buff[DBUS_MAXIMUM_MATCH_RULE_LENGTH];
	snprintf(buff, sizeof(buff), "destination='%s',path='%s',interface='%s',member='%s'", u, path, interface, name);

	dbus_bus_add_match(dc->conn, buff, &err);

	if(dbus_error_is_set(&err)) {
		E_WARNING(E_STRLOC ": Adding method match failed: %s, %s\n", err.name, err.message);
		dbus_error_free(&err);
		return;
	}

	dc->method_matches++;
}

void EdbusConnection::register_object(const char* path) {
	if(!dc || !dc->conn)
		return;

	E_ASSERT(path != NULL);
	E_ASSERT(EdbusObjectPath::valid_path(path) && "Got invalid object path");

	if(!have_registered_object(dc, path))
		dc->object_list.push_back(path);
}

void EdbusConnection::unregister_object(const char* path) {
	if(!dc || !dc->conn)
		return;

	E_ASSERT(path != NULL);
	E_ASSERT(EdbusObjectPath::valid_path(path) && "Got invalid object path");

	if(dc->object_list.empty())
		return;

	ObjectPathListIter it = dc->object_list.begin(), it_end = dc->object_list.end();
	for(; it != it_end; ++it) {
		if(strcmp(*it, path) == 0) {
			dc->object_list.erase(it);
			break;
		}
	}
}

void EdbusConnection::setup_listener_with_fltk(void) {
	if(!dc || !dc->conn)
		return;

	setup_filter();

	/* allocate our watch list */
	dc->watch_list = new WatchList;

	dbus_connection_set_watch_functions(dc->conn, edbus_add_watch, edbus_remove_watch, edbus_toggle_watch, dc, 0);
	dbus_connection_set_timeout_functions(dc->conn, edbus_add_timeout, edbus_remove_timeout, 
			edbus_toggle_timeout, dc, 0);
	dbus_connection_set_wakeup_main_function(dc->conn, edbus_wakeup_main, 0, 0);
}

void EdbusConnection::setup_listener(void) {
	if(!dc || !dc->conn)
		return;

	setup_filter();
}

int EdbusConnection::wait(int timout_ms) {
	if(!dc || !dc->conn)
		return 0;

	return dbus_connection_read_write_dispatch(dc->conn, timout_ms);
}

EDELIB_NS_END
