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

#ifndef __EDBUSMESSAGE_H__
#define __EDBUSMESSAGE_H__

#include "List.h"
#include "EdbusData.h"

struct DBusMessage;

EDELIB_NS_BEGIN

class  EdbusConnection;
struct EdbusMessageImpl;

/**
 * \ingroup dbus
 *
 * \class EdbusMessage
 * \brief Data transporter for D-Bus
 *
 * EdbusMessage is essentially the way you send and receive a bunch of EdbusData objects.
 * You can see it as e-mail message: EdbusData represents a content you wrote or read and 
 * EdbusMessage is that content plus headers with origin, destination and message type.
 *
 * Knowing that, EdbusMessage can be:
 * - signal, created with create_signal(); it represents D-Bus signal
 * - method call, created with create_method_call(); calls a method on service objects
 * - method reply, created with create_reply(); sends a reply on called method
 * - error reply, created with create_error_reply(); sends a error reply on called method
 *
 * As you will see from below, signals do not have destination; they are broadcasted
 * over session or system bus and all clients (connected on session or system bus) that 
 * do not filter signals will receive it.
 *
 * On other hand, methods (calls and replies) do have destination and they are sent directly
 * to the client. This is the main difference between signals and methods.
 *
 * Creating a specific EdbusMessage type you do via one of the <em>create_</em> members.
 * If you call them on already created EdbusMessage with some content, that content will
 * be discarded and EdbusMessage will become appropriate type.
 *
 * When you create a message, you wants to add some data. This is done via append() member
 * like:
 * \code
 *   EdbusMessage m;
 *   m.create_signal("/org/example/SignalObject", "org.example.Signal", "MySignal");
 *   m.append(EdbusData::from_int32(34));
 *   m.append(EdbusData::from_bool(true));
 *   m.append(EdbusData::from_string("some string"));
 * \endcode
 *
 * EdbusMessage have operator<<() that makes adding data much more... sexy ;-)
 *
 * There is above sample via this opeartor:
 * \code
 *   EdbusMessage m;
 *   m.create_signal("/org/example/SignalObject", "org.example.Signal", "MySignal");
 *   m << EdbusData::from_int32(32) << EdbusData::from_bool(true) << EdbusData::from_string("some string");
 * \endcode
 *
 * Getting data from message is done via iterator. Since iterator do not know what type
 * it points to, you <b>must</b> check it manually before calling appropriate EdbusData function
 * to fetch concrete data or bad things will happen (assertion will be upon you).
 *
 * Let say you received above message and wants to get a content from it. You will do it like:
 * \code
 *   EdbusMessage::iterator it = m.begin(), it_end = m.end();
 *
 *   for(; it != it_end; ++it) {
 *     if((*it).is_int32())
 *       printf("Got int with: %i\n", (*it).to_int32());
 *
 *     if((*it).is_bool())
 *       printf("Got bool with: %i\n", (*it).to_bool());
 *
 *     if((*it).is_string())
 *       printf("Got string with: %s\n", (*it).to_string());
 *   }
 * \endcode
 *
 * Changing (or removing) already present data inside EdbusMessage is done via iterator. With this you
 * can e.g. modify received message and send it again. 
 */

class EDELIB_API EdbusMessage {
private:
	friend class EdbusConnection;

	EdbusMessageImpl* dm;
	list<EdbusData> msg_content;

	void from_dbus_message(DBusMessage* m);
	DBusMessage* to_dbus_message(void) const;

	EdbusMessage(const EdbusMessage&);
	EdbusMessage& operator=(const EdbusMessage&);

public:
	/**
	 * Declare EdbusMessage iterator
	 */
	typedef list<EdbusData>::iterator iterator;

	/**
	 * Declare EdbusMessage const iterator
	 */
	typedef list<EdbusData>::const_iterator const_iterator;

	/**
	 * Create an empty EdbusMessage object. Nothing will be allocated
	 * until you call one of the <em>create_</em> members. Until that, message
	 * is marked as invalid and EdbusConnection will refuse to send it
	 */
	EdbusMessage();

	/**
	 * Create an EdbusMessage from DBusMessage. This is used to simplify
	 * internals and you should not use it directly
	 */
	EdbusMessage(DBusMessage* msg);

	/**
	 * Clears internal data
	 */
	~EdbusMessage();

	/**
	 * Create a signal message
	 *
	 * \param path is destination object path
	 * \param interface is destination interface name
	 * \param name is signal name
	 */
	void create_signal(const char* path, const char* interface, const char* name);

	/**
	 * Create a method call
	 *
	 * \param service is destination service name
	 * \param path is destination object path
	 * \param interface is destination interface name
	 * \param method is method to be called
	 */
	void create_method_call(const char* service, const char* path, const char* interface, const char* method);

	/**
	 * Create a reply for method
	 *
	 * \param replying_to is a message to be replyed
	 */
	void create_reply(const EdbusMessage& replying_to);

	/**
	 * Create error reply
	 *
	 * \param replying_to is a message to be replyed
	 * \param errmsg is error string to be sent
	 */
	void create_error_reply(const EdbusMessage& replying_to, const char* errmsg);

	/**
	 * Clears all EdbusMessage data (including message headers with destination, type and etc.) 
	 * and marks it as invalid. If you want to send this object again, make sure to re-create
	 * it again with one of <em>create_</em> members.
	 *
	 * This is since DBus internally queues messages after they are sent (if there is need to 
	 * re-send them again) there is no way to clear message content without destroying headers
	 */
	void clear_all(void);

	/**
	 * Returns true if current message is signal type
	 */
	bool is_signal(void);

	/**
	 * Returns true if current message is method call type
	 */
	bool is_method_call(void);

	/**
	 * Returns true if current message is error reply type
	 */
	bool is_error_reply(const char* errmsg);

	/**
	 * Set object path for destination
	 * It will do nothing if one of the <em>create_</em> members are called before
	 */
	void path(const char* np);

	/**
	 * Get object path for destination
	 * It will return NULL if one of the <em>create_</em> members are not called before
	 */
	const char* path(void) const;

	/**
	 * Set interface name for destination. 
	 * It will do nothing if one of the <em>create_</em> members are not called before
	 */
	void interface(const char* ni);

	/**
	 * Get interface name for destination
	 * It will return NULL if one of the <em>create_</em> members are not called before
	 */
	const char* interface(void) const;

	/**
	 * Sets the message's destination.
	 *
	 * The destination is the name of another connection on the bus 
	 * and may be either the unique name assigned by the bus to each 
	 * connection, or a well-known name specified in advance
	 *
	 * It will do nothing if one of the <em>create_</em> members are not called before
	 */
	void destination(const char* nd);

	/**
	 * Get a message destination
	 * It will return NULL if one of the <em>create_</em> members are not called before
	 */
	const char* destination(void) const;

	/**
	 * Set method name to be called.
	 * \note This function can be used to set signal name too
	 *
	 * It will do nothing if one of the <em>create_</em> members are not called before
	 */
	void member(const char* nm);

	/**
	 * Get method or signal name from message
	 * It will return NULL if one of the <em>create_</em> members are not called before
	 */
	const char* member(void) const;

	/**
	 * Sets the message sender.
	 *
	 * The sender must be a valid bus name as defined in the D-Bus 
	 * specification
	 *
	 * It will do nothing if one of the <em>create_</em> members are not called before
	 */
	void sender(const char* ns);

	/**
	 * Gets the unique name of the connection which originated this 
	 * message, or NULL if unknown or inapplicable.
	 *
	 * The sender is filled in by the message bus.
	 *
	 * It will return NULL if one of the <em>create_</em> members are not called before
	 */
	const char* sender(void) const;

	/**
	 * Returns the signature of this message. You will not need this
	 * unless you know what returned value means.
	 *
	 * A signature will contain only reasonable content when you receive message
	 * via one of the callbacks set inside EdbusConnection.
	 *
	 * It will return NULL if one of the <em>create_</em> members are not called before
	 */
	const char* signature(void) const;

	/**
	 * Append EdbusData object in message
	 */
	void append(const EdbusData& data) { msg_content.push_back(data); }

	/**
	 * Returns iterator at the message start. It points to the first element
	 */
	iterator begin(void) { return msg_content.begin(); }

	/**
	 * Returns const iterator at the message start. It points to the first element
	 */
	const_iterator begin(void) const { return msg_content.begin(); }

	/**
	 * Returns iterator at the message end. It <b>does not</b> points to
	 * the last element, but element after the last, and you must not dereferce it
	 */
	iterator end(void) { return msg_content.end(); }

	/**
	 * Returns const iterator at the message end. It <b>does not</b> points to
	 * the last element, but element after the last, and you must not dereferce it
	 */
	const_iterator end(void) const { return msg_content.end(); }

	/**
	 * Returns the size of EdbusMessage content
	 */
	unsigned int size(void) const { return msg_content.size(); }
};

/**
 * \ingroup dbus
 *
 * A convinient shortcut for EdbusMessage::append(). You can use it as:
 * \code
 *   EdbusMessage m;
 *   m << EdbusData::from_int32(4) << EdbusData::from_string("sample string") << EdbusData::from_bool(true);
 * \endcode
 */
inline EdbusMessage& operator<<(EdbusMessage& m, const EdbusData& val) {
	m.append(val);
	return m;
}

EDELIB_NS_END

#endif
