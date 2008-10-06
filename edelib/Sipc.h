/*
 * $Id$
 *
 * A very simple IPC
 * Part of edelib.
 * Copyright (c) 2008 EDE Authors.
 *
 * This program is licenced under terms of the 
 * GNU General Public Licence version 2 or newer.
 * See COPYING for details.
 */

#ifndef __SIPC_H__
#define __SIPC_H__

#include "econfig.h"

EDELIB_NS_BEGIN

#ifndef SKIP_DOCS
class SipcServerPrivate;
class SipcClientPrivate;
#endif


/**
 * A callback type for SipcServer message callbacks
 */
typedef void (*SipcCallback)(const char*, void*);

/**
 * \class SipcServer
 * \brief Simple IPC server
 *
 * Sipc stands for Simple IPC (Inter Process Communication) and is
 * very lighweight communication system between two processes by exchanging
 * character messages based on UNIX domain sockets.
 *
 * Sipc main purpose is to allow quick message exchanging when messages
 * are simple (like simple notifications for refresh or quit) and there is no 
 * need for D-BUS, as heavier alternative (see EdbusConnection).
 * It also minimizes linkage depenedencies (in D-BUS case) and can provide
 * some sort of IPC support in places where D-BUS libraries does not exists.
 *
 * SipcServer will listen incomming messages and will report them via
 * registered callback. Before listening starts, you have to <i>register</i>
 * name via request_name() adding prefix parameter; this name will be used
 * by clients to connects to (same like port numbers, as you concluded already ;)).
 *
 * If name is already taken, request_name() will fail and you will have to
 * try with another one.
 *
 * To listen for incomming connections, make sure you call listener_wait().
 *
 * Here is the short sample:
 * \code
 *   void message_cb(const char* msg, void* arg) {
 *     // sent messages will be in msg parameter
 *   }
 *
 *   // in some function...
 *   SipcServer s;
 *   if(!s.request_name("demo"))
 *     // fail
 *   s.callback(message_cb);
 *
 *   // now listen infinitely for connections
 *   while(1)
 *     listener_wait();
 * \endcode
 *
 * Bidirectional communication is not possible (a case when server wants to reply);
 * for that D-BUS exists :P
 */
class EDELIB_API SipcServer {
	private:
		SipcServerPrivate* priv;

		SipcServer(const SipcServer&);
		SipcServer& operator=(const SipcServer&);
	public:
		/**
		 * Constructor; prepares internal data
		 */
		SipcServer();

		/**
		 * Cleans internal stuff, closes connections
		 */
		~SipcServer();

		/**
		 * Register name to be listening on. The name should be unique
		 * and not taken by other listener.
		 *
		 * \return false if failed or true otherwise
		 * \param prefix is name clients will use for connection
		 */
		bool request_name(const char* prefix);

		/**
		 * Register callback, called when message arrives.
		 *
		 * \param cb is callback
		 * \param data is optional data passet to the callback
		 */
		void callback(SipcCallback cb, void* data);
};

/**
 * \class SipcClient
 * \brief Simple IPC client
 *
 * A client side of this simple IPC implementation. Clients will connects
 * on the names servers are using and send textual messages to them.
 *
 * Here is the sample:
 * \code
 *   SipcClient c;
 *   if(!c.connect("demo"))
 *    // fail
 *   c.send("howdy");
 * \endcode
 *
 * \note Message length is currently is limited to 1024 bytes.
 */
class EDELIB_API SipcClient {
	private:
		SipcClientPrivate* priv;

		SipcClient(const SipcClient&);
		SipcClient& operator=(const SipcClient&);
	public:
		/**
		 * Constructor; prepares internal data
		 */
		SipcClient();

		/**
		 * Cleans internal stuff, closes connections
		 */
		~SipcClient();

		/**
		 * Connects to the given name
		 *
		 * \return false if name does not exists or some error occured
		 * \param prefix is name to be connected to
		 */
		bool connect(const char* prefix);

		/**
		 * Sends an message
		 *
		 * \param msg is textual message
		 */
		void send(const char* msg);
};

EDELIB_NS_END

#endif
