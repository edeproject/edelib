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

#ifndef __EDELIB_EDBUSCONNECTION_H__
#define __EDELIB_EDBUSCONNECTION_H__

#include "EdbusMessage.h"
#include "EdbusError.h"

EDELIB_NS_BEGIN

/**
 * \defgroup dbus D-Bus classes and functions
 */

/**
 * \ingroup dbus
 * \def EDBUS_INTROSPECTION_DTD
 * This macro is DTD header for DBus introspection XML. Useful only if you are going to construct own
 * introspection XML reply.
 */
#define EDBUS_INTROSPECTION_DTD \
	"<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\" \n" \
	"\"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">\n\n"

/**
 * \ingroup dbus
 * \enum EdbusConnectionType
 * \brief Represents connection either to system or session bus
 */
enum EdbusConnectionType {
	EDBUS_SYSTEM,   ///< System bus
	EDBUS_SESSION   ///< Session bus
};

/**
 * \ingroup dbus
 * \enum EdbusNameMode
 * \brief What to do when known name is acquired
 */
enum EdbusNameMode {
	EDBUS_NAME_NO_REPLACE       = 0,  ///< Do not replace existing name
	EDBUS_NAME_ALLOW_REPLACE    = 1,  ///< Existing name can be replaced
	EDBUS_NAME_REPLACE_EXISTING = 2   ///< Force replacement
};

/**
 * \ingroup dbus
 * A callback type for method and signal callbacks
 */
typedef int (*EdbusCallback)(const EdbusMessage*, void*);

#ifndef SKIP_DOCS
struct EdbusConnImpl;
#endif

/**
 * \ingroup dbus
 * \class EdbusConnection
 * \brief D-Bus connection and data sender
 *
 * This is the main class representing connection to the D-Bus daemon. Besides doing actual connection,
 * this class is used also to send and receive messages.
 *
 * EdbusConnection implements <i>message loop</i>; it will wait until message arrived or until message was 
 * send. Because of this, EdbusConnection implements two way of looping:
 *  - ordinary loop
 *  - loop via FLTK
 *
 * Ordinary loop is recomended only in non-gui applications; it will block until connection was closed or
 * object was destroyed in different way. \see EdbusConnection::setup_listener()
 *
 * Looping via FLTK (relying on <i>Fl::run()</i> or <i>Fl::wait()</i>) will be used for gui applications;
 * gui components will receive own events and callbacks, but in the same time EdbusConnection will also
 * be able to listen and send messages. \see EdbusConnection::setup_listener_with_fltk()
 *
 * To connect to the bus, connect() must be called first with one of EdbusConnectionType values. All messages 
 * will be sent to that bus type until disconnect() was called or object were destroyed.
 *
 * In case you want to listen for calls, you would acquire name via EdbusConnection::request_name(), on what
 * clients will connect. You would also register object via EdbusConnection::register_object().
 *
 * With EdbusConnection you can send two types of messages:
 *  - signals
 *  - methods
 *
 * Messages are recived via callbacks. Edbus splits callbacks on:
 *  - signal callbacks (EdbusConnection::signal_callback())
 *  - method callbacks (EdbusConnection::method_callback())
 *
 * Here is the example of one application that will listen "Foo" signal:
 * \code
 *  int sig_cb(const EdbusMessage* m, void*) {
 *    printf("Got signal: %s : %s : %s\n", m->path(), m->interface(), m->member());
 *    
 *    // this means signal were processed so dbus can discard it
 *    return 1;
 *  }
 *
 *  int main() {
 *    EdbusConnection conn;
 *    if(!conn.connect(EDBUS_SESSION))
 *      // fail
 *
 *    if(!conn.request_name("org.test.Server"))
 *      // fail
 *
 *    srv.register_object("/org/test/Server/Foo");
 *    srv.signal_callback(sig_cb, 0);
 *
 *    // looping stuff
 *    srv.setup_listener()
 *    while(srv.wait(1000))
 *      ;
 *
 *    return 0;
 *  }
 * \endcode
 *
 * Here is application that will send "Foo" signal:
 * \code
 *  int main() {
 *    EdbusConnection conn;
 *    if(!conn.connect(EDBUS_SESSION))
 *      // fail
 *
 *    // create empty signal message without any parameters
 *    EdbusMessage msg;
 *    msg.create_signal("/org/test/Server/Foo", "org.test.Signal", "Foo");
 *
 *    // send it
 *    conn.send(msg);
 *    return 0;
 *  }
 * \endcode
 */
class EDELIB_API EdbusConnection {
private:
	EdbusConnImpl* dc;
	void setup_filter(void);

	E_DISABLE_CLASS_COPY(EdbusConnection)
public:
	/**
	 * Creates empty object. You can't do anything usefull with
	 * it unless call connect() after.
	 */
	EdbusConnection();

	/**
	 * Destroys object. Also disconnect from bus if connection is alive.
	 */
	~EdbusConnection();

	/**
	 * Connects to either session or system bus.
	 *
	 * \return true if connected, otherwise false and EdbusError object will be set
	 * \param ctype says what connection is requested
	 */
	bool connect(EdbusConnectionType ctype);

	/**
	 * Disconnects from bus. Also clears internal watchers so you can issue connect() again.
	 */
	void disconnect(void);

	/**
	 * Return if this object is connected. This method will not only check the status of internal connection holder,
	 * like if <em>connect()</em> was called, but will also check if current connection is alive, consulting DBus API call.
	 *
	 * \return true if connected or false if not
	 */
	bool connected(void);

	/**
	 * Sends a message.
	 *
	 * \return true if succesfully got reply
	 * \param content is message to be send
	 *
	 * \note if send() fails, EdbusError will not be set since D-BUS default send function does not set error
	 */
	bool send(const EdbusMessage& content);

	/**
	 * Call remote method and wait for reply. It will block untill reply is arrived
	 * or timer exceeded.
	 *
	 * \return true if succesfully got reply or false if not and EdbusError object (get via error()) will be set
	 * \param content is message to be send
	 * \param timeout_ms is waiting time for arrival in milliseconds
	 * \param ret will be filled with reply content if this function returns true
	 */
	bool send_with_reply_and_block(const EdbusMessage& content, int timeout_ms, EdbusMessage& ret);

	/**
	 * Try to set readable name, e.g. <em>org.equinoxproject.Listener</em>. 
	 * If EdbusConnection object wants to accept messages, clients will send them to this name.
	 * 
	 * This function also can be used to assure unique name in the bus; if you set EDBUS_NAME_NO_REPLACE
	 * every other call (from program itself or external program) for request with the same name will
	 * return false value. This does not mean you will not receive a bus messages; they would be queued and
	 * dispatched among listeners with the same name.
	 *
	 * With other flags (EDBUS_NAME_REPLACE_EXISTING and EDBUS_NAME_NO_REPLACE) you can achieve smooth name
	 * replacement. For example, if you have program service1 that registered org.example.Service and
	 * (during runtime) want to replace it with upgraded or modified service2, simply setting in service2
	 * EDBUS_NAME_REPLACE_EXISTING flag, and EDBUS_NAME_ALLOW_REPLACE service1 will do the job. Then, service1
	 * will receive <em>NameLost</em> from org.freedesktop.DBus interface and can choose to quit or do
	 * something else.
	 *
	 * \em mode flags can be OR-ed, so EDBUS_NAME_ALLOW_REPLACE | EDBUS_NAME_REPLACE_EXISTING can work
	 * in both programs (in given example), except <em>NameLost</em> will receive program that was started
	 * first, but request_name() will not return false. OR-ing with EDBUS_NAME_NO_REPLACE have no much sense, 
	 * and if is detected, it is considered as plain EDBUS_NAME_NO_REPLACE.
	 *
	 * \return true if got requested name or false if not and EdbusError object (get via error()) will be set
	 * \param name is name to be requested
	 * \param mode is what to do when requested name already exists
	 */
	bool request_name(const char* name, int mode = EDBUS_NAME_NO_REPLACE);

	/**
	 * Get unique name for this connection. Returned value have sense only for D-BUS.
	 */
	const char* unique_name(void);

	/**
	 * Register callback for signal arrival. When signal is arrived, it will be passed
	 * to the callback as EdbusMessage where you can extract content via it's members.
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
	 * If you registered at least one object, data not send to it will be ignored. On other hand,
	 * if none object was added, other objects receiving data will be reported here too.
	 *
	 * \note This function only stores pointer to the string, so <em>make sure</em> it is in static memory
	 */
	void register_object(const char* path);

	/**
	 * Unregister already registered object.
	 */
	void unregister_object(const char* path);

	/**
	 * Setup listening stuff. After this, EdbusConnection object will be ready to accept
	 * requests.
	 *
	 * \note You want to call wait() after this so listener can await requests in the loop.
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

	/**
	 * Return last error that happened; if error wasn't set, returned value will be NULL. Error can be
	 * invalid, so it must be checked too. \see EdbusError::valid()
	 */
	EdbusError* error(void);
};

EDELIB_NS_END
#endif
