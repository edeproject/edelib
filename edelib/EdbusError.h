/*
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

#ifndef __EDELIB_EDBUSERROR_H__
#define __EDELIB_EDBUSERROR_H__

#include "edelib-global.h"

struct DBusError;

EDELIB_NS_BEGIN

/**
 * \ingroup dbus
 * \enum EdbusErrorType
 * \brief EdbusError error type
 */
enum EdbusErrorType {
	EDBUS_ERROR_INVALID,              ///< empty error
	EDBUS_ERROR_FAILED,               ///< generic failure
	EDBUS_ERROR_NO_MEMORY,            ///< out of memory
	EDBUS_ERROR_SERVICE_UNKNOWN,      ///< address service is neither connected nor can be activated
	EDBUS_ERROR_NAME_HAS_NO_OWNER,    ///< a non-unique name used in message is not known
	EDBUS_ERROR_NO_REPLY,             ///< expected reply not received
	EDBUS_ERROR_IO_ERROR,             ///< IO error during operation
	EDBUS_ERROR_BAD_ADDRESS,          ///< malformed address to be connected to
	EDBUS_ERROR_NOT_SUPPORTED,        ///< requested operation not supported
	EDBUS_ERROR_LIMITS_EXCEEDED,      ///< resource reached it's limit
	EDBUS_ERROR_ACCESS_DENIED,        ///< not enough permissions to execute or access operation
	EDBUS_ERROR_AUTH_FAILED,          ///< authentication failed
	EDBUS_ERROR_NO_SERVER,            ///< unable to connect to the D-Bus server
	EDBUS_ERROR_TIMEOUT,              ///< an timeout occured during operation
	EDBUS_ERROR_NO_NETWORK,           ///< transport channel network not available
	EDBUS_ERROR_DISCONNECTED,         ///< trying to perform operation on unconnected connection
	EDBUS_ERROR_INVALID_ARGS,         ///< invalid arguments for method call
	EDBUS_ERROR_FILE_NOT_FOUND,       ///< a file needed for operation not present
	EDBUS_ERROR_FILE_EXISTS,          ///< file exists, but operation does not allow overwriting
	EDBUS_ERROR_UNKNOWN_METHOD,       ///< unknown method
	EDBUS_ERROR_TIMED_OUT,            ///< an operation timed out
	EDBUS_ERROR_INVALID_SIGNATURE,    ///< invalid signature for message
	EDBUS_ERROR_USER_DEFINED          ///< user defined error
};

#ifndef SKIP_DOCS
struct EdbusErrorImpl;
#endif

/**
 * \ingroup dbus
 * \class EdbusError
 * \brief A class representing D-Bus error
 */
class EDELIB_API EdbusError {
private:
	EdbusErrorImpl* impl;
	void dispose(void);
public:
	/**
	 * Create invalid error object
	 */
	EdbusError();

	/**
	 * Destroys object
	 */
	~EdbusError();

	/**
	 * Create error object from DBusError
	 */
	EdbusError(const DBusError* err);

	/**
	 * Create error object from another object
	 */
	EdbusError(const EdbusError& other);

	/**
	 * Assing another object
	 */
	EdbusError& operator=(const EdbusError& other);

	/**
	 * Explicitly create error with given EdbusErrorType and text
	 */
	EdbusError(EdbusErrorType t, const char* msg);

	/**
	 * Returns error type
	 */
	EdbusErrorType type(void) const;

	/**
	 * Returns the D-Bus error name. It will be NULL if error is invalid
	 */
	const char* name(void) const;

	/**
	 * Returns a string describing the error. It will be NULL if error is invalid
	 */
	const char* message(void) const;

	/**
	 * Returns true if error object contains valid type (any except EDBUS_ERROR_INVALID)
	 */
	bool valid(void) const { return type() != EDBUS_ERROR_INVALID; }
};

EDELIB_NS_END
#endif
