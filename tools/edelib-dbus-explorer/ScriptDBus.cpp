
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

#include <string.h>
#include <edelib/Scheme.h>
#include <edelib/Debug.h>
#include <edelib/EdbusMessage.h>
#include <edelib/EdbusData.h>

#include "ScriptDBus.h"

EDELIB_NS_USING(EdbusConnection)
EDELIB_NS_USING(EdbusMessage)
EDELIB_NS_USING(EdbusData)

static EdbusConnection **curr_con = NULL;

#define STR_CMP(s1, s2) (strcmp((s1), (s2)) == 0)

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

static bool edbus_data_from_pair(scheme *s, pointer type, pointer val, EdbusData &ret) {
	E_RETURN_VAL_IF_FAIL(edelib_scheme_is_symbol(s, type), false);
	char *sym = edelib_scheme_symname(s, type);

	if(STR_CMP(sym, ":int16") || STR_CMP(sym, ":uint16") ||
	   STR_CMP(sym, ":int32") || STR_CMP(sym, ":uint32") ||
	   STR_CMP(sym, ":byte") || STR_CMP(sym, ":bool"))
	{
		if(edelib_scheme_is_int(s, val)) {
			int iv = edelib_scheme_int_value(s, val);

			if (STR_CMP(sym, ":byte"))       ret = EdbusData::from_byte((byte_t)iv);
			else if(STR_CMP(sym, ":bool"))   ret = EdbusData::from_bool((bool)iv);
			else if(STR_CMP(sym, ":int16"))  ret = EdbusData::from_int16((int16_t)iv);
			else if(STR_CMP(sym, ":uint16")) ret = EdbusData::from_uint16((uint16_t)iv);
			else if(STR_CMP(sym, ":int32"))  ret = EdbusData::from_int32((int32_t)iv);
			else if(STR_CMP(sym, ":uint32")) ret = EdbusData::from_uint32((uint32_t)iv);

			return true;
		}

		LOCAL_SCHEME_ERROR(s, "Type marked as byte/bool/integer but the value is not");
		return false;
	}

	if(STR_CMP(sym, ":double")) {
		if(edelib_scheme_is_double(s, val)) {
			double dv = edelib_scheme_double_value(s, val);
			ret = EdbusData::from_double(dv);
			return true;
		}

		LOCAL_SCHEME_ERROR(s, "Type marked as double but the value is not");
		return false;
	}

	if(STR_CMP(sym, ":string")) {
		if(edelib_scheme_is_string(s, val)) {
			char *sv = edelib_scheme_string_value(s, val);
			ret = EdbusData::from_string(sv);
			return true;
		}

		LOCAL_SCHEME_ERROR(s, "Type marked as string but the value is not");
		return false;
	}

	if(STR_CMP(sym, ":array")) {
		if(edelib_scheme_is_vector(s, val)) {
		}

		LOCAL_SCHEME_ERROR(s, "Type marked as array but the value is not vector");
		return false;
	}

	return true;
}

/* accept list in form '(:int32 3 :string 4 :bool 5 ...) and construct EdbusMessage from it */
static EdbusMessage& edbus_message_from_list(scheme *s, pointer args, EdbusMessage &msg) {
}

pointer script_bus_signal(scheme *s, pointer args) {
	LOCAL_SCHEME_RETURN_IF_NOT_CONNECTED(s);
	int len = list_length(s, args);
	LOCAL_SCHEME_RETURN_IF_FAIL(s, (len == 3 || len == 4), "Expecting at least 3 and max 4 arguments");

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
					  edelib_scheme_string_value(s, interface),
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
