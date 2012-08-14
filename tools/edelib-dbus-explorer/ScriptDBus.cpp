
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
#include <edelib/EdbusList.h>
#include <edelib/EdbusDict.h>

#include "ScriptDBus.h"

EDELIB_NS_USING(EdbusConnection)
EDELIB_NS_USING(EdbusMessage)
EDELIB_NS_USING(EdbusData)
EDELIB_NS_USING(EdbusObjectPath)
EDELIB_NS_USING(EdbusList)
EDELIB_NS_USING(EdbusDict)
EDELIB_NS_USING(EDBUS_TYPE_INVALID)

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

/* this could be a part of edelib scheme */
static pointer reverse_in_place(scheme *sc, pointer term, pointer lst) {
	pointer p = lst, result = term, q;

	while(p != sc->NIL) {
		q = edelib_scheme_pair_cdr(sc, p);
		edelib_scheme_cdr_set(sc, p, result);
		result = p;
		p = q;
	}

	return result;
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
			LOCAL_SCHEME_ERROR(s, "Not implemented yet");
			return false;
		}

		LOCAL_SCHEME_ERROR(s, "Type marked as array but the value is not vector");
		return false;
	}

	if(STR_CMP(sym, ":list")) {
		if(edelib_scheme_is_pair(s, val)) {
			LOCAL_SCHEME_ERROR(s, "Not implemented yet");
			return false;
		}

		LOCAL_SCHEME_ERROR(s, "Type marked as list but the value is not list");
		return false;
	}

	return true;
}

#define RETURN_FROM_EDBUS_TO_SCHEME_AS_INT(scm, object, type)	\
do {															\
	if(object.is_ ## type ()) {									\
		type ## _t v = object.to_ ## type ();					\
		return edelib_scheme_mk_int(scm, (long)v);				\
	}															\
} while(0)

static pointer edbus_data_to_scheme_object(scheme *s, EdbusData &data) {
	if(data.is_bool()) {
		bool v = data.to_bool();
		return edelib_scheme_mk_int(s, (long)v);
	}

	if(data.is_char()) {
		char c = data.to_char();
		return edelib_scheme_mk_character(s, (int)c);
	}

	if(data.is_double())
		return edelib_scheme_mk_double(s, data.to_double());

	if(data.is_string())
		return edelib_scheme_mk_string(s, data.to_string());

	if(data.is_object_path()) {
		EdbusObjectPath v = data.to_object_path();
		return edelib_scheme_mk_string(s, v.path());
	}

	if(data.is_variant()) {
		/* recurse, as variant can be anything */
		EdbusData v = data.to_variant().value;
		return edbus_data_to_scheme_object(s, v);
	}

	RETURN_FROM_EDBUS_TO_SCHEME_AS_INT(s, data, byte);
	RETURN_FROM_EDBUS_TO_SCHEME_AS_INT(s, data, int16);
	RETURN_FROM_EDBUS_TO_SCHEME_AS_INT(s, data, uint16);
	RETURN_FROM_EDBUS_TO_SCHEME_AS_INT(s, data, int32);
	RETURN_FROM_EDBUS_TO_SCHEME_AS_INT(s, data, uint32);
	RETURN_FROM_EDBUS_TO_SCHEME_AS_INT(s, data, int64);
	RETURN_FROM_EDBUS_TO_SCHEME_AS_INT(s, data, uint64);

	/* array is represented as scheme vector */
	if(data.is_array()) {
		EdbusList arr = data.to_array();
		EdbusList::const_iterator it = arr.begin(), ite = arr.end();
		int i = 0;

		pointer elem, vec = edelib_scheme_mk_vector(s, arr.size());
		for(; it != ite; ++it, ++i) {
			/* element can be anything, so recurse again; also remove constness as elements will not be modified */
			elem = edbus_data_to_scheme_object(s, (EdbusData&)*it);
			edelib_scheme_vector_elem_set(s, vec, i, elem);
		}

		return vec;
	}

	/* struct is represented as scheme list */
	if(data.is_struct()) {
		EdbusList v = data.to_struct();
		EdbusList::const_iterator it = v.begin(), ite = v.end();

		pointer elem, ret = s->NIL;
		for(; it != ite; ++it) {
			elem = edbus_data_to_scheme_object(s, (EdbusData&)*it);
			ret = edelib_scheme_cons(s, elem, ret);
		}

		return reverse_in_place(s, s->NIL, ret);
	}

	/* dictionary is represented as list of list, like: '((key val) (key val) ...) so we can use assoc */
	if(data.is_dict()) {
		EdbusDict v = data.to_dict();	
		EdbusDict::const_iterator it = v.begin(), ite = v.end();

		pointer key, val, kv_pair = s->NIL, ret = s->NIL;
		for(; it != ite; ++it) {
			key = edbus_data_to_scheme_object(s, (EdbusData&)(it->key));
			val = edbus_data_to_scheme_object(s, (EdbusData&)(it->value));

			/* add them in reverse order so we don't have to reverse it */
			kv_pair = edelib_scheme_cons(s, val, kv_pair);
			kv_pair = edelib_scheme_cons(s, key, kv_pair);

			ret = edelib_scheme_cons(s, kv_pair, ret);
		}

		return ret;
	}

	return s->F;
}

/* accept list in form '(:int32 3 :string 4 :bool 5 ...) and construct EdbusMessage from it */
static bool edbus_message_params_from_list(scheme *s, pointer args, EdbusMessage &msg) {
	int len = list_length(s, args);

	if(E_UNLIKELY(len < 1)) {
		LOCAL_SCHEME_ERROR(s, "Arguments and their types are not specified.");
		return false;
	}

	if(E_UNLIKELY((len % 2) != 0)) {
		LOCAL_SCHEME_ERROR(s, "Mismatched number or arguments and their types. Somewhere you are either missing argument type or type does not have variable");
		return false;
	}

	/* start scanning in pair */
	pointer t, v, tmp = args;

	for(; tmp && tmp != s->NIL; tmp = edelib_scheme_pair_cdr(s, tmp)) {
		t = edelib_scheme_pair_car(s, tmp);

		tmp = edelib_scheme_pair_cdr(s, tmp);
		E_ASSERT(tmp != s->NIL && "Wrong code. Always expected pair");

		v = edelib_scheme_pair_car(s, tmp);
		E_ASSERT(v != s->NIL && "Wrong code. Always expected pair");

		EdbusData data;
		if(edbus_data_from_pair(s, t, v, data)) {
			msg << data;
		} else {
			/* let caller knows something fails */
			return false;
		}
	}

	/* all fine */
	return true;
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

pointer script_bus_method_call(scheme *s, pointer args) {
	LOCAL_SCHEME_RETURN_IF_NOT_CONNECTED(s);

	int len = list_length(s, args);
	LOCAL_SCHEME_RETURN_IF_FAIL(s, (len >= 4), "Expecting at least 4 arguments");

	pointer service, path, interface, name, params;
	service = edelib_scheme_pair_car(s, args);
	LOCAL_SCHEME_RETURN_IF_FAIL(s, edelib_scheme_is_string(s, service), "First argument must be a string");

	args = edelib_scheme_pair_cdr(s, args);
	path = edelib_scheme_pair_car(s, args);
	LOCAL_SCHEME_RETURN_IF_FAIL(s, edelib_scheme_is_string(s, path), "Second argument must be a string");

	args = edelib_scheme_pair_cdr(s, args);
	interface = edelib_scheme_pair_car(s, args);
	LOCAL_SCHEME_RETURN_IF_FAIL(s, edelib_scheme_is_string(s, interface), "Third argument must be a string");

	args = edelib_scheme_pair_cdr(s, args);
	name = edelib_scheme_pair_car(s, args);
	LOCAL_SCHEME_RETURN_IF_FAIL(s, edelib_scheme_is_string(s, name), "Fourth argument must be a string");

	EdbusMessage msg;

	msg.create_method_call(edelib_scheme_string_value(s, service),
						   edelib_scheme_string_value(s, path),
						   edelib_scheme_string_value(s, interface),
						   edelib_scheme_string_value(s, name));

	args = edelib_scheme_pair_cdr(s, args);
	if(args != s->NIL && ((params = edelib_scheme_pair_car(s, args)) != s->NIL)) {
		/* fail if we can't get arguments */
		if(!edbus_message_params_from_list(s, params, msg))
			return s->F;
	}

	EdbusMessage reply;
	if((*curr_con)->send_with_reply_and_block(msg, 1000, reply)) {
		if(reply.size() == 0) {
			E_DEBUG(E_STRLOC ": got reply with 0 objects\n");
			return s->T;
		}

		/* scan reply and construct scheme objects */
		EdbusData item = *(reply.begin());
		return edbus_data_to_scheme_object(s, item);
	}

	return s->F;
}

void script_dbus_load(scheme *s, EdbusConnection **con) {
	curr_con = con;

	EDELIB_SCHEME_DEFINE2(s, script_bus_signal, "dbus-signal",
"Send DBus signal with given parameters. Parameters are object path, interface\n\
and signal name. DBus signals does not return value so this function will return\n\
either #t or #f depending if signal successfully sent.");

	/*
	EDELIB_SCHEME_DEFINE2(s, script_bus_get_property, "dbus-get-property",
"Return value of named property on given interface. If failed to fetch property value\n\
return #f.");
	*/

	EDELIB_SCHEME_DEFINE2(s, script_bus_method_call, "dbus-call",
"Call DBus method with given arguments. This call will wait for reply and return result as scheme object.");
}
