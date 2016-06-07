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

#include <string.h>
#include <stdlib.h>
#include <dbus/dbus.h>

#include <edelib/EdbusError.h>
#include <edelib/Debug.h>

EDELIB_NS_BEGIN

#define COPY_DBUS_ERROR(e) strdup(e)
#define STR_CMP(s1, s2) (strcmp(s1, s2) == 0)

struct EdbusErrorImpl {
	char* name;
	char* msg;
	EdbusErrorType type;
	unsigned int refs;
};

static char* from_edbus_errortype(EdbusErrorType t) {
	switch(t) {
		case EDBUS_ERROR_INVALID:
		case EDBUS_ERROR_USER_DEFINED:
			E_ASSERT(0 && "Should not get here");
			return NULL;
		case EDBUS_ERROR_FAILED:
			return COPY_DBUS_ERROR(DBUS_ERROR_FAILED);
		case EDBUS_ERROR_NO_MEMORY:
			return COPY_DBUS_ERROR(DBUS_ERROR_NO_MEMORY);
		case EDBUS_ERROR_SERVICE_UNKNOWN:
			return COPY_DBUS_ERROR(DBUS_ERROR_SERVICE_UNKNOWN);
		case EDBUS_ERROR_NAME_HAS_NO_OWNER:
			return COPY_DBUS_ERROR(DBUS_ERROR_NAME_HAS_NO_OWNER);
		case EDBUS_ERROR_NO_REPLY:
			return COPY_DBUS_ERROR(DBUS_ERROR_NO_REPLY);
		case EDBUS_ERROR_IO_ERROR:
			return COPY_DBUS_ERROR(DBUS_ERROR_IO_ERROR);
		case EDBUS_ERROR_BAD_ADDRESS:
			return COPY_DBUS_ERROR(DBUS_ERROR_BAD_ADDRESS);
		case EDBUS_ERROR_NOT_SUPPORTED:
			return COPY_DBUS_ERROR(DBUS_ERROR_NOT_SUPPORTED);
		case EDBUS_ERROR_LIMITS_EXCEEDED:
			return COPY_DBUS_ERROR(DBUS_ERROR_LIMITS_EXCEEDED);
		case EDBUS_ERROR_ACCESS_DENIED:
			return COPY_DBUS_ERROR(DBUS_ERROR_ACCESS_DENIED);
		case EDBUS_ERROR_AUTH_FAILED:
			return COPY_DBUS_ERROR(DBUS_ERROR_AUTH_FAILED);
		case EDBUS_ERROR_NO_SERVER:
			return COPY_DBUS_ERROR(DBUS_ERROR_NO_SERVER);
		case EDBUS_ERROR_TIMEOUT:
			return COPY_DBUS_ERROR(DBUS_ERROR_TIMEOUT);
		case EDBUS_ERROR_NO_NETWORK:
			return COPY_DBUS_ERROR(DBUS_ERROR_NO_NETWORK);
		case EDBUS_ERROR_DISCONNECTED:
			return COPY_DBUS_ERROR(DBUS_ERROR_DISCONNECTED);
		case EDBUS_ERROR_INVALID_ARGS:
			return COPY_DBUS_ERROR(DBUS_ERROR_INVALID_ARGS);
		case EDBUS_ERROR_FILE_NOT_FOUND:
			return COPY_DBUS_ERROR(DBUS_ERROR_FILE_NOT_FOUND);
		case EDBUS_ERROR_FILE_EXISTS:
			return COPY_DBUS_ERROR(DBUS_ERROR_FILE_EXISTS);
		case EDBUS_ERROR_UNKNOWN_METHOD:
			return COPY_DBUS_ERROR(DBUS_ERROR_UNKNOWN_METHOD);
		case EDBUS_ERROR_TIMED_OUT:
			return COPY_DBUS_ERROR(DBUS_ERROR_TIMED_OUT);
		case EDBUS_ERROR_INVALID_SIGNATURE:
			return COPY_DBUS_ERROR(DBUS_ERROR_INVALID_SIGNATURE);
	}

	/* should not ever be reached */
	E_ASSERT(0 && "Got wrong error type ?!?");
	return NULL;
}

EdbusErrorType from_dbus_errorstr(const char* msg) {
	if(!msg)
		return EDBUS_ERROR_INVALID;

	if(STR_CMP(msg, DBUS_ERROR_FAILED))
		return EDBUS_ERROR_FAILED;
	if(STR_CMP(msg, DBUS_ERROR_NO_MEMORY))
		return EDBUS_ERROR_NO_MEMORY;
	if(STR_CMP(msg, DBUS_ERROR_SERVICE_UNKNOWN))
		return EDBUS_ERROR_SERVICE_UNKNOWN;
	if(STR_CMP(msg, DBUS_ERROR_NAME_HAS_NO_OWNER))
		return EDBUS_ERROR_NAME_HAS_NO_OWNER;
	if(STR_CMP(msg, DBUS_ERROR_NO_REPLY))
		return EDBUS_ERROR_NO_REPLY;
	if(STR_CMP(msg, DBUS_ERROR_IO_ERROR))
		return EDBUS_ERROR_IO_ERROR;
	if(STR_CMP(msg, DBUS_ERROR_BAD_ADDRESS))
		return EDBUS_ERROR_BAD_ADDRESS;
	if(STR_CMP(msg, DBUS_ERROR_NOT_SUPPORTED))
		return EDBUS_ERROR_NOT_SUPPORTED;
	if(STR_CMP(msg, DBUS_ERROR_LIMITS_EXCEEDED))
		return EDBUS_ERROR_LIMITS_EXCEEDED;
	if(STR_CMP(msg, DBUS_ERROR_ACCESS_DENIED))
		return EDBUS_ERROR_ACCESS_DENIED;
	if(STR_CMP(msg, DBUS_ERROR_AUTH_FAILED))
		return EDBUS_ERROR_AUTH_FAILED;
	if(STR_CMP(msg, DBUS_ERROR_NO_SERVER))
		return EDBUS_ERROR_NO_SERVER;
	if(STR_CMP(msg, DBUS_ERROR_TIMEOUT))
		return EDBUS_ERROR_TIMEOUT;
	if(STR_CMP(msg, DBUS_ERROR_NO_NETWORK))
		return EDBUS_ERROR_NO_NETWORK;
	if(STR_CMP(msg, DBUS_ERROR_DISCONNECTED))
		return EDBUS_ERROR_DISCONNECTED;
	if(STR_CMP(msg, DBUS_ERROR_INVALID_ARGS))
		return EDBUS_ERROR_INVALID_ARGS;
	if(STR_CMP(msg, DBUS_ERROR_FILE_NOT_FOUND))
		return EDBUS_ERROR_FILE_NOT_FOUND;
	if(STR_CMP(msg, DBUS_ERROR_FILE_EXISTS))
		return EDBUS_ERROR_FILE_EXISTS;
	if(STR_CMP(msg, DBUS_ERROR_UNKNOWN_METHOD))
		return EDBUS_ERROR_UNKNOWN_METHOD;
	if(STR_CMP(msg, DBUS_ERROR_TIMED_OUT))
		return EDBUS_ERROR_TIMED_OUT;
	if(STR_CMP(msg, DBUS_ERROR_INVALID_SIGNATURE))
		return EDBUS_ERROR_INVALID_SIGNATURE;

	return EDBUS_ERROR_INVALID;
}

EdbusError::EdbusError() : impl(NULL) {
	impl = new EdbusErrorImpl;
	impl->name = NULL;
	impl->msg = NULL;
	impl->type = EDBUS_ERROR_INVALID;
	impl->refs = 1;
}

EdbusError::~EdbusError() {
	if(!impl)
		return;

	impl->refs--;
	if(impl->refs == 0)
		dispose();
}

void EdbusError::dispose(void) {
	if(!impl)
		return;
	if(impl->name)
		free(impl->name);
	if(impl->msg)
		free(impl->msg);

	delete impl;
	impl = NULL;
}

EdbusError::EdbusError(const DBusError* err) {
	impl = new EdbusErrorImpl;
	impl->name = NULL;
	impl->msg = NULL;
	impl->type = EDBUS_ERROR_INVALID;
	impl->refs = 1;

	if(!err || !dbus_error_is_set(err))
		return;

	impl->name = COPY_DBUS_ERROR(err->name);
	impl->msg = COPY_DBUS_ERROR(err->message);
	impl->type = from_dbus_errorstr(err->name);
}

EdbusError::EdbusError(const EdbusError& other) {
	if(this == &other)
		return;

	impl = other.impl;
	other.impl->refs++;
}

EdbusError::EdbusError(EdbusErrorType t, const char* msg) {
	impl = new EdbusErrorImpl;
	impl->name = from_edbus_errortype(t);
	impl->msg = strdup(msg);
	impl->type = t;
	impl->refs = 1;
}

EdbusError& EdbusError::operator=(const EdbusError& other) {
	other.impl->refs++;
	impl->refs--;

	if(impl->refs == 0)
		dispose();

	impl = other.impl;
	return *this;
}

EdbusErrorType EdbusError::type(void) const {
	E_ASSERT(impl != NULL);
	return impl->type;
}

const char* EdbusError::name() const {
	E_ASSERT(impl != NULL);
	return impl->name;
}

const char* EdbusError::message() const {
	E_ASSERT(impl != NULL);
	return impl->msg;
}

EDELIB_NS_END
