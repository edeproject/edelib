
/*
 * $Id: String.h 2839 2009-09-28 11:36:20Z karijes $
 *
 * Copyright (c) 2005-2012 edelib authors
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

#include <edelib/Scheme.h>
#include <edelib/Debug.h>
#include <edelib/EdbusMessage.h>
#include "ScriptDBus.h"

EDELIB_NS_USING(EdbusConnection)
EDELIB_NS_USING(EdbusMessage)

static EdbusConnection **curr_con = NULL;

/* TODO: find better way to handle this, or add edelib_scheme_error inside Scheme.h */
#define LOCAL_SCHEME_ERROR(scm, str) scheme_load_string(scm, "(error " #str ")");

#define LOCAL_SCHEME_RETURN_IF_FAIL(scm, expr, str)\
do {											   \
	if(E_LIKELY(expr)) { }						   \
	else {										   \
		LOCAL_SCHEME_ERROR(scm, str);			   \
		return scm->F;							   \
	}											   \
} while(0)

#define LOCAL_SCHEME_RETURN_IF_NOT_CONNECTED(s)	\
	LOCAL_SCHEME_RETURN_IF_FAIL(s, (curr_con && *curr_con && (*curr_con)->connected()), "Not connected to bus")

/* this could be a part of edelib scheme */
static int list_length(scheme *s, pointer args) {
	int n = 0;
	pointer tmp = args;
	while(tmp != s->NIL) {
		tmp = edelib_scheme_pair_cdr(s, tmp);
		n++;
	}

	return n;
}

pointer script_bus_signal(scheme *s, pointer args) {
	LOCAL_SCHEME_RETURN_IF_NOT_CONNECTED(s);
	LOCAL_SCHEME_RETURN_IF_FAIL(s, list_length(s, args) == 3, "Expecting 3 arguments");

	pointer path, interface, name;
	path = edelib_scheme_pair_car(s, args);
	LOCAL_SCHEME_RETURN_IF_FAIL(s, edelib_scheme_is_string(s, path), "First argument must be a string");

	args = edelib_scheme_pair_cdr(s, args);
	interface = edelib_scheme_pair_car(s, args);
	LOCAL_SCHEME_RETURN_IF_FAIL(s, edelib_scheme_is_string(s, interface), "Second argument must be a string");

	args = edelib_scheme_pair_cdr(s, args);
	name = edelib_scheme_pair_car(s, args);
	LOCAL_SCHEME_RETURN_IF_FAIL(s, edelib_scheme_is_string(s, name), "Third argument must be a string");

	EdbusMessage msg;
	msg.create_signal(edelib_scheme_string_value(s, path),
					  edelib_scheme_string_value(s, args),
					  edelib_scheme_string_value(s, name));
	
	if((*curr_con)->send(msg)) return s->T;
	return s->F;
}

void script_dbus_load(scheme *s, EdbusConnection **con) {
	curr_con = con;

	EDELIB_SCHEME_DEFINE2(s, script_bus_signal, "dbus-signal",
						  "Send DBus signal with given parameters. Parameters are object path, interface\n "
						  "and signal name. DBus signals does not return value so this function will return\n "
						  "either #t or #f depending if signal successfully sent.");
}
