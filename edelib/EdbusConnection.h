/*
 * $Id$
 *
 * D-Bus stuff
 * Part of edelib.
 * Copyright (c) 2008 EDE Authors.
 *
 * This program is licenced under terms of the 
 * GNU General Public Licence version 2 or newer.
 * See COPYING for details.
 */

#ifndef __EDBUSCONNECTION_H__
#define __EDBUSCONNECTION_H__

#include "EdbusMessage.h"

EDELIB_NS_BEGIN

/**
 * \ingroup dbus
 * \enum EdbusConnectionType
 * \brief Represents connection either to system or session bus
 */
enum EdbusConnectionType {
	EDBUS_SYSTEM,
	EDBUS_SESSION
};

/**
 * \ingroup dbus
 * \enum EdbusNameMode
 * \brief What to do when known name is acquired
 */
enum EdbusNameMode {
	EDBUS_NAME_NO_REPLACE        = 0,  ///< Do not replace existing name
	EDBUS_NAME_ALLOW_REPLACE     = 1,  ///< Existing name can be replaced
	EDBUS_NAME_REPLACE_EXISTITNG = 2   ///< Force replacement
};

/**
 * \ingroup dbus
 * A callback type for method and signal callbacks
 */
typedef int (*EdbusCallback)(const EdbusMessage*, void*);

/**
 * \ingroup dbus
 * \class EdbusCallbackItem
 * \brief An item for callback table
 */
struct EdbusCallbackItem {
	/** object path */
	const char*   path;
	/** interface name */
	const char*   interface;
	/** method or signal name */
	const char*   name;
	/** a callback executed on method or signal name match */
	EdbusCallback callback;
	/** optinal data passed to the callback */
	void*         data;
};

#ifndef SKIP_DOCS
struct EdbusConnImpl;
#endif

/**
 * \page dbusdoc A short info about D-Bus
 *
 * \section dbusintro Introduction
 *
 * D-Bus is an IPC (Inter Process Communication) protocol with low overhead. It is designed
 * to unify communication between various applications, desktop environments and system properties.
 *
 * D-Bus is a peer-to-peer and client-server protocol; this means you can communicate directly either with
 * main daemon (<em>dbus-daemon</em>) or with some application that represents a service (then is
 * done indirectly via dbus-daemon).
 *
 * When you issue connection to the daemon, you will use one of two connection types:
 * - system bus - communicate with services running on system level
 * - session bus - communicate with services and applications started after you logged in. This is
 *   a common way of communication in desktop environments
 *
 * Since D-Bus is a binary protocol, all data is marshalled into binary form, and unmarshalled when 
 * receiver get it. With this bindinding, this is done transparently.
 *
 * Contrary to the other IPC mechanisms and protocols, D-Bus is relatively simple one, but flexible. As you
 * can see from EdbusData, D-Bus allows various types to be send: a basic ones, like int or char and
 * complex ones, like arrays, structures or variants. With this, you can send almost any type.
 *
 * \section dbussyntax Syntax convention
 *
 * D-Bus brings some common terms from other IPC systems and introduce more. They are following:
 *
 * \subsection dbusservice Services
 */

/**
 * \defgroup dbus D-Bus classes and functions
 */

/**
 * \ingroup dbus
 * \class EdbusConnection
 * \brief D-Bus connection and data sender
 */
class EdbusConnection {
	private:
		EdbusConnImpl* dc;
		void setup_filter(void);

		EdbusConnection(const EdbusConnection&);
		EdbusConnection& operator=(const EdbusConnection&);

	public:
		/**
		 * Creates empty object. You can't do anything usefull with
		 * it unless call connect() after
		 */
		EdbusConnection();

		/**
		 * Destroys object. Also disconnect from bus if connection is alive
		 */
		~EdbusConnection();

		/**
		 * Connects to either session or system bus
		 *
		 * \return true if connected, otherwise false
		 * \param ctype says what connection is requested
		 */
		bool connect(EdbusConnectionType ctype);

		/**
		 * Disconnects from bus. Also clears internal watchers so you can issue
		 * connect() again.
		 *
		 * \todo returning here is obsolete
		 * \return always true
		 */
		bool disconnect(void);

		/**
		 * Sends a message
		 *
		 * \return true if sending was succesfull
		 * \param content is message to be send
		 */
		bool send(const EdbusMessage& content);

		/**
		 * Call remote method and wait for reply. It will block untill reply is arrived
		 * or timer exceeded
		 *
		 * \return true if succesfully got reply
		 * \param content is message to be send
		 * \param timeout_ms is waiting time for arrival in milliseconds
		 * \param ret will be filled with reply content if this function returns true
		 */
		bool send_with_reply_and_block(const EdbusMessage& content, int timeout_ms, EdbusMessage& ret);

		/**
		 * Try to set readable name, e.g. <em>org.equinoxproject.Listener</em>. 
		 * If EdbusConnection object wants to accept messages, clients will send them to this name
		 *
		 * \return true if got requested name
		 * \param name is name to be requested
		 * \param mode is what to do when requested name already exists
		 */
		bool request_name(const char* name, int mode = EDBUS_NAME_NO_REPLACE);

		/**
		 * Get unique name for this connection
		 */
		const char* unique_name(void);

		/**
		 * Register callback for signal arrival. When signal is arrived, it will be passed
		 * to the callback as EdbusMessage where you can extract content via it's members
		 *
		 * \param cb is callback
		 * \param data is optional data that will be passed to the callback
		 */
		void signal_callback(EdbusCallback cb, void* data);

		/**
		 * Register callback for method call. When peer requests a call from this connection,
		 * this function will be called. EdbusMessage parameter is arguments for method call.
		 *
		 * \param cb is callback
		 * \param data is optional data that will be passed to the callback
		 */
		void method_callback(EdbusCallback cb, void* data);

		/**
		 * Register callback table for signals. If arrived signal matches item in table,
		 * it will call callback from it.
		 *
		 * It works on first match; an item that is found, it's callback will be called, no
		 * matter if later another callback is defined.
		 *
		 * \note signal table have precedence over plain signal_callback(); this means if you
		 * set a table, and later set signal callback via signal_callback(), later will be ignored
		 */
		void signal_callback_table(EdbusCallbackItem* table, unsigned int sz);

		/**
		 * Set opional data to item in signal table. This data will be send to callback function
		 */
		void signal_callback_table_data(unsigned int pos, void* data);

		/**
		 * Removes registered table
		 */
		void remove_signal_callback_table(void) { signal_callback_table(NULL, 0); }

		/**
		 * Register callback table for method calls. The same rules applies as for signal_callback_table()
		 */
		void method_callback_table(EdbusCallbackItem* table, unsigned int sz);

		/**
		 * Set opional data to item in method call table. This data will be send to callback function
		 */
		void method_callback_table_data(unsigned int pos, void* data);

		/**
		 * Removes registered table
		 */
		void remove_method_callback_table(void) { method_callback_table(NULL, 0); }

		/**
		 * Install matcher for received signals. All signals that matches to the given
		 * path, interface and name will be reported via signal callback registered with signal_callback().
		 *
		 * You can install more matchers.
		 */
		void add_signal_match(const char* path, const char* interface, const char* name);

		/**
		 * Install matcher for requested method calls. All method calls that matches to the given
		 * path, interface and name will be reported via method callback registered with method_callback().
		 *
		 * This is not too much suitable as signal matchers since you definitely wants to listen
		 * all method requests that client sends directly to you.
		 *
		 * You can install more matchers.
		 */
		void add_method_match(const char* path, const char* interface, const char* name);

		/**
		 * Register objects this connection will server. A path must be valid D-Bus object path and this
		 * function will assert if otherwise. If object already registered, it will not be added any more.
		 *
		 * You can register more that one object.
		 *
		 * In order to receive data, you must register at least one object
		 *
		 * \note This function only stores pointer to the string, so <em>make sure</em> it is in static memory
		 */
		void register_object(const char* path);

		/**
		 * Unregister already registered object
		 */
		void unregister_object(const char* path);

		/**
		 * Setup listening stuff. After this, EdbusConnection object will be ready to accept
		 * requests.
		 *
		 * \note You want to call wait() after this so listener can await requests in loop
		 */
		void setup_listener(void);

		/**
		 * The same as setup_listener() except it will be integrated in FLTK application without
		 * blocking FLTK event loops. After this you will call either <em>Fl::run()</em> of <em>Fl::wait()</em>
		 */
		void setup_listener_with_fltk(void);

		/**
		 * Run listener and wait clients connection or signals. This is blocking call and is assumed
		 * it will not be run inside GUI application. For FLTK GUI application, you should use 
		 * setup_listener_with_fltk().
		 *
		 * This function can be used similar to the <em>Fl::wait()</em>, like:
		 * \code
		 *   // init objects and etc.
		 *   conn.setup_listener();
		 *   while(conn.wait(1000))
		 *     ;
		 * \endcode
		 *
		 * \param timeout_ms is time in milliseconds to wait for connections
		 */
		int wait(int timeout_ms);
};

EDELIB_NS_END

#endif
