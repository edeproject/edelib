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

#include <stdlib.h>
#include <stdio.h>
#include <dbus/dbus.h>

#include <edelib/EdbusMessage.h>
#include <edelib/EdbusDict.h>
#include <edelib/EdbusList.h>
#include <edelib/Debug.h>
#include <edelib/String.h>

EDELIB_NS_BEGIN

struct EdbusMessageImpl {
	DBusMessage* msg;
};

static void to_dbus_iter_from_edbusdata_type(DBusMessageIter* parent_it, const EdbusData& data);

static const char* from_edbusdata_type_to_dbus_type_string(EdbusDataType t) {
	switch(t) {
		case EDBUS_TYPE_BYTE:
			return DBUS_TYPE_BYTE_AS_STRING;
		case EDBUS_TYPE_BOOL:
			return DBUS_TYPE_BOOLEAN_AS_STRING;
		case EDBUS_TYPE_INT16:
			return DBUS_TYPE_INT16_AS_STRING;
		case EDBUS_TYPE_UINT16:
			return DBUS_TYPE_UINT16_AS_STRING;
		case EDBUS_TYPE_INT32:
			return DBUS_TYPE_INT32_AS_STRING;
		case EDBUS_TYPE_UINT32:
			return DBUS_TYPE_UINT32_AS_STRING;
		case EDBUS_TYPE_INT64:
			return DBUS_TYPE_INT64_AS_STRING;
		case EDBUS_TYPE_UINT64:
			return DBUS_TYPE_UINT64_AS_STRING;
		case EDBUS_TYPE_DOUBLE:
			return DBUS_TYPE_DOUBLE_AS_STRING;
		case EDBUS_TYPE_STRING:
			return DBUS_TYPE_STRING_AS_STRING;
		case EDBUS_TYPE_OBJECT_PATH:
			return DBUS_TYPE_OBJECT_PATH_AS_STRING;
		case EDBUS_TYPE_ARRAY:
			return DBUS_TYPE_ARRAY_AS_STRING;
		case EDBUS_TYPE_STRUCT:
			return DBUS_TYPE_STRUCT_AS_STRING;
		case EDBUS_TYPE_DICT:
			return DBUS_TYPE_DICT_ENTRY_AS_STRING;
		case EDBUS_TYPE_VARIANT:
			return DBUS_TYPE_VARIANT_AS_STRING;
		case EDBUS_TYPE_INVALID:
			return 0;
	}

	/* should be never reached in valid code */
	printf("Got unknown (%i) type. Marking it as invalid...\n", t);
	return 0;
}

/* 
 * Builds a signature for subtypes, e.g. array, struct or dict elements.
 * This function will be called on container subtypes, not container itself
 */
static void build_signature(const EdbusData& data, String& sig) {
	if(data.is_dict()) {
		EdbusDict dd = data.to_dict();

		if(dd.size() < 1)
			return;

		sig += DBUS_TYPE_ARRAY_AS_STRING;
		sig += DBUS_DICT_ENTRY_BEGIN_CHAR;

		EdbusDict::const_iterator it = dd.begin();
		/* always basic */
		sig += from_edbusdata_type_to_dbus_type_string((*it).key.type());

		if(EdbusData::basic_type((*it).value))
			sig += from_edbusdata_type_to_dbus_type_string((*it).value.type());
		else {
			/* recurse for more */
			build_signature((*it).value, sig);
		}

		sig += DBUS_DICT_ENTRY_END_CHAR;
	} else if(data.is_array()) {
		EdbusList arr = data.to_array();

		if(arr.size() < 1)
			return;

		sig += DBUS_TYPE_ARRAY_AS_STRING;
		EdbusList::const_iterator it = arr.begin();
		
		if(EdbusData::basic_type(*it))
			sig += from_edbusdata_type_to_dbus_type_string((*it).type());
		else {
			/* recurse for more */
			build_signature((*it), sig);
		}
	} else if(data.is_struct()) {
		EdbusList s = data.to_struct();

		if(s.size() < 1)
			return;

		sig += DBUS_STRUCT_BEGIN_CHAR;
		EdbusList::const_iterator it = s.begin(), it_end = s.end();

		for(; it != it_end; ++it) {
			if(EdbusData::basic_type(*it))
				sig += from_edbusdata_type_to_dbus_type_string((*it).type());
			else {
				/* recurse for more */
				build_signature((*it), sig);
			}
		}

		sig += DBUS_STRUCT_END_CHAR;
	} else {
		/* here can be any other basic type and/or variant */
		sig += from_edbusdata_type_to_dbus_type_string(data.type());
	}
}

static void to_dbus_iter_from_basic_type(DBusMessageIter* msg_it, const EdbusData& data) {
	if(data.is_bool()) {
		/* force it so DBus knows real size */
		const dbus_bool_t v = data.to_bool();
		dbus_message_iter_append_basic(msg_it, DBUS_TYPE_BOOLEAN, &v);
		return;
	}
	
	if(data.is_byte()) {
		byte_t v = data.to_byte();
		dbus_message_iter_append_basic(msg_it, DBUS_TYPE_BYTE, &v);
		return;
	} 
	
	if(data.is_int16()) {
		int16_t v = data.to_int16();
		dbus_message_iter_append_basic(msg_it, DBUS_TYPE_INT16, &v);
		return;
	} 
	
	if(data.is_uint16()) {
		uint16_t v = data.to_uint16();
		dbus_message_iter_append_basic(msg_it, DBUS_TYPE_UINT16, &v);
		return;
	} 
	
	if(data.is_int32()) {
		int32_t v = data.to_int32();
		dbus_message_iter_append_basic(msg_it, DBUS_TYPE_INT32, &v);
		return;
	} 
	
	if(data.is_uint32()) {
		uint32_t v = data.to_uint32();
		dbus_message_iter_append_basic(msg_it, DBUS_TYPE_UINT32, &v);
		return;
	} 
	
	if(data.is_int64()) {
		int64_t v = data.to_int64();
		dbus_message_iter_append_basic(msg_it, DBUS_TYPE_INT64, &v);
		return;
	} 
	
	if(data.is_uint64()) {
		uint64_t v = data.to_uint64();
		dbus_message_iter_append_basic(msg_it, DBUS_TYPE_UINT64, &v);
		return;
	} 
	
	if(data.is_double()) {
		double v = data.to_double();
		dbus_message_iter_append_basic(msg_it, DBUS_TYPE_DOUBLE, &v);
		return;
	} 
	
	if(data.is_string()) {
		const char* v = data.to_string();
		dbus_message_iter_append_basic(msg_it, DBUS_TYPE_STRING, &v);
		return;
	} 

	E_ASSERT(0 && "Got type as basic but it isn't");
}

static void to_dbus_iter_from_dict(DBusMessageIter* parent_it, const EdbusData& data) {
	E_ASSERT(data.is_dict());

	EdbusDict dict = data.to_dict();

	/* TODO: allow empty containers ??? */
	if(dict.size() < 1)
		return;

	/*
	 * TODO: check signature size in DBus spec (or use edelib::String here)
	 *
	 * Also, this part needs a better code, especially in case when value signature
	 * is needed. Value can be another dict, struct or array of dicts, struct, variants
	 * and etc. so I need better singature builder.
	 */
	char sig[256];
	const char* key_sig = from_edbusdata_type_to_dbus_type_string(dict.key_type());
	//const char* value_sig = from_edbusdata_type_to_dbus_type_string(dict.value_type());
#if 0	
	const char* value_sig;
	if(dict.value_type() == EDBUS_TYPE_DICT)
		value_sig = "a{ss}";
	else
		value_sig = from_edbusdata_type_to_dbus_type_string(dict.value_type());
#endif
	const char* value_sig;
	String ss;

	if(dict.value_type_is_container()) {
		/*
		 * We already have correct header that represents dict array. Now
		 * go and recurse into dict value container
		 */
		EdbusDict::const_iterator first = dict.begin();

		build_signature((*first).value, ss);
		value_sig = ss.c_str();
	} else
		value_sig = from_edbusdata_type_to_dbus_type_string(dict.value_type());


	/*
	 * Dicts are serialized as array of dict entries. We first build signature for array
	 * elements, then iterate over EdbusDict entries and from key/value pairs construct a 
	 * DBus dict entry then append that entry to the array
	 *
	 * A signature for dictionary will be 'a{TYPE TYPE}'. dbus_message_iter_open_container() will
	 * append 'a' to the signature.
	 */
	snprintf(sig, sizeof(sig), "%c%s%s%c", 
			DBUS_DICT_ENTRY_BEGIN_CHAR,
			key_sig,
			value_sig,
			DBUS_DICT_ENTRY_END_CHAR);

	printf("Dict entry signature: %s\n", sig);

	DBusMessageIter sub;
	dbus_message_iter_open_container(parent_it, DBUS_TYPE_ARRAY, sig, &sub);

	EdbusDict::const_iterator it = dict.begin(), it_end = dict.end();
	for(; it != it_end; ++it) {
		DBusMessageIter dict_entry_iter;

		dbus_message_iter_open_container(&sub, DBUS_TYPE_DICT_ENTRY, 0, &dict_entry_iter);

		/* 
		 * append key; it is always basic type
		 * TODO: here should be assertion check
		 */
		to_dbus_iter_from_basic_type(&dict_entry_iter, (*it).key);

		/* append value, can be any type */
		to_dbus_iter_from_edbusdata_type(&dict_entry_iter, (*it).value);

		dbus_message_iter_close_container(&sub, &dict_entry_iter);
	}

	dbus_message_iter_close_container(parent_it, &sub);
}

static void to_dbus_iter_from_array(DBusMessageIter* parent_it, const EdbusData& data) {
	E_ASSERT(data.is_array());

	/*
	 * Marshalling arrays is much simpler than e.g. dict; we already know all elements
	 * are the same type. The only tricky case is when array elements are another array, dict
	 * or struct so signature, where build_signature() comes in
	 */
	EdbusList arr = data.to_array();

	/* TODO: allow empty containers ??? */
	if(arr.size() < 1)
		return;

	EdbusList::const_iterator it = arr.begin(), it_end = arr.end();

	const char* value_sig;
	String ss;

	if(arr.value_type_is_container()) {
		build_signature(*it, ss);
		value_sig = ss.c_str();
	} else
		value_sig = from_edbusdata_type_to_dbus_type_string(arr.value_type());


	printf("Array entry signature: %s\n", value_sig);

	/* 
	 * dbus_message_iter_open_container() will by default append container
	 * signature (at the start of signature array)
	 */
	DBusMessageIter sub;
	dbus_message_iter_open_container(parent_it, DBUS_TYPE_ARRAY, value_sig, &sub);

	while(it != it_end) {
		to_dbus_iter_from_edbusdata_type(&sub, *it);
		++it;
	}

	dbus_message_iter_close_container(parent_it, &sub);
}

static void to_dbus_iter_from_struct(DBusMessageIter* parent_it, const EdbusData& data) {
	E_ASSERT(data.is_struct());

	EdbusList s = data.to_struct();
	EdbusList::const_iterator it = s.begin(), it_end = s.end();

	DBusMessageIter sub;
	/* structs does not require signature for types they contains, the same as dicts */
	dbus_message_iter_open_container(parent_it, DBUS_TYPE_STRUCT, NULL, &sub);

	while(it != it_end) {
		to_dbus_iter_from_edbusdata_type(&sub, *it);
		++it;
	}

	dbus_message_iter_close_container(parent_it, &sub);
}

static void to_dbus_iter_from_variant(DBusMessageIter* parent_it, const EdbusData& data) {
	E_ASSERT(data.is_variant());

	EdbusVariant var = data.to_variant();
	/* hm... really needed ? */
	if(!var.value.is_valid())
		return;

	const char* value_sig;
	String ss;

	if(!EdbusData::basic_type(var.value)) {
		build_signature(var.value, ss);
		value_sig = ss.c_str();
	} else
		value_sig = from_edbusdata_type_to_dbus_type_string(var.value.type());

	printf("variant entry is %s\n", value_sig);

	DBusMessageIter sub;
	dbus_message_iter_open_container(parent_it, DBUS_TYPE_VARIANT, value_sig, &sub);

	to_dbus_iter_from_edbusdata_type(&sub, var.value);

	dbus_message_iter_close_container(parent_it, &sub);
}

/* marshall from EdbusData type to DBus type via iterator */
static void to_dbus_iter_from_edbusdata_type(DBusMessageIter* parent_it, const EdbusData& data) {
	if(EdbusData::basic_type(data)) {
		to_dbus_iter_from_basic_type(parent_it, data);
		return;
	}

	if(data.is_dict()) {
		to_dbus_iter_from_dict(parent_it, data);
		return;
	}

	if(data.is_array()) {
		to_dbus_iter_from_array(parent_it, data);
		return;
	}

	if(data.is_struct()) {
		to_dbus_iter_from_struct(parent_it, data);
		return;
	}

	if(data.is_variant()) {
		to_dbus_iter_from_variant(parent_it, data);
		return;
	}

	E_ASSERT(0 && "This should not be ever reached!");
}

/* unmarshall from DBus type to EdbusData type */
static void from_dbus_iter_to_edbusdata_type(DBusMessageIter* iter, EdbusData& data) {
	int dtype = dbus_message_iter_get_arg_type(iter);

	if(dtype == DBUS_TYPE_BOOLEAN) {
		dbus_bool_t v;
		dbus_message_iter_get_basic(iter, &v);
		data = EdbusData::from_bool(v);
		return;
	} 
	
	if(dtype == DBUS_TYPE_BYTE) {
		/* DBus does not have dbus_byte_t so use our definition */
		byte_t v;
		dbus_message_iter_get_basic(iter, &v);
		data = EdbusData::from_byte(v);
		return;
	} 
	
	if(dtype == DBUS_TYPE_INT16) {
		dbus_int16_t v;
		dbus_message_iter_get_basic(iter, &v);
		data = EdbusData::from_int16(v);
		return;
	} 
	
	if(dtype == DBUS_TYPE_UINT16) {
		dbus_uint16_t v;
		dbus_message_iter_get_basic(iter, &v);
		data = EdbusData::from_uint16(v);
		return;
	} 
	
	if(dtype == DBUS_TYPE_INT32) {
		dbus_int32_t v;
		dbus_message_iter_get_basic(iter, &v);
		data = EdbusData::from_int32(v);
		return;
	} 
	
	if(dtype == DBUS_TYPE_UINT32) {
		dbus_uint32_t v;
		dbus_message_iter_get_basic(iter, &v);
		data = EdbusData::from_uint32(v);
		return;
	} 
	
	if(dtype == DBUS_TYPE_INT64) {
		dbus_int64_t v;
		dbus_message_iter_get_basic(iter, &v);
		data = EdbusData::from_int64(v);
		return;
	} 
	
	if(dtype == DBUS_TYPE_UINT64) {
		dbus_uint64_t v;
		dbus_message_iter_get_basic(iter, &v);
		data = EdbusData::from_uint64(v);
		return;
	} 
	
	if(dtype == DBUS_TYPE_STRING) {
		/* TODO: strings are UTF-8 encoded */
		const char* v;
		dbus_message_iter_get_basic(iter, &v);
		data = EdbusData::from_string(v);
		return;
	} 
	
	if(dtype == DBUS_TYPE_OBJECT_PATH) {
		/* TODO: strings are UTF-8 encoded */
		const char* v;
		dbus_message_iter_get_basic(iter, &v);
		data = EdbusData::from_object_path(v);
		return;
	}

	if(dtype == DBUS_TYPE_ARRAY) {
		int arr_type = dbus_message_iter_get_element_type(iter);

		if(arr_type == DBUS_TYPE_DICT_ENTRY) {
			DBusMessageIter array_iter;
			dbus_message_iter_recurse(iter, &array_iter);

			/* check type of array entries */
			if(dbus_message_iter_get_arg_type(&array_iter) == DBUS_TYPE_INVALID) {
				puts("Invalid in dict array !?");

				/* if fails, construct empty dict */
				EdbusDict empty;
				data = EdbusData::from_dict(empty);
				return;
			}

			EdbusDict ret;
			EdbusData key, value;

			/* 
			 * Now iterate over the array and iterate over each array
			 * item to get key/value pair. Although key is always a basic DBus type,
			 * for simplification we will recurse to from_dbus_iter_to_edbusdata_type().
			 *
			 * Value can be another dict or else, so recursion is needed
			 */
			while(dbus_message_iter_get_arg_type(&array_iter) == DBUS_TYPE_DICT_ENTRY) {
				DBusMessageIter kv;

				dbus_message_iter_recurse(&array_iter, &kv);

				if(dbus_message_iter_get_arg_type(&kv) != DBUS_TYPE_INVALID) {
					from_dbus_iter_to_edbusdata_type(&kv, key);

					/* expected next item is value so increate iterator to it */
					dbus_message_iter_next(&kv);

					if(dbus_message_iter_get_arg_type(&kv) != DBUS_TYPE_INVALID) {
						from_dbus_iter_to_edbusdata_type(&kv, value);

						/* got key and value, store it then */
						ret.append(key, value);
					}
				}

				dbus_message_iter_next(&array_iter);
			}

			data = EdbusData::from_dict(ret);
			return;
		}

		/* not dictionary, then this is real array */
		EdbusList arr = EdbusList::create_array();
		EdbusData val;
		DBusMessageIter array_it;

		dbus_message_iter_recurse(iter, &array_it);

		while(dbus_message_iter_get_arg_type(&array_it) != DBUS_TYPE_INVALID) {
			from_dbus_iter_to_edbusdata_type(&array_it, val);
			arr.append(val);

			dbus_message_iter_next(&array_it);
		}

		data = EdbusData::from_array(arr);
		return;
	}

	if(dtype == DBUS_TYPE_VARIANT) {
		EdbusVariant var;
		DBusMessageIter sub;

		dbus_message_iter_recurse(iter, &sub);
		from_dbus_iter_to_edbusdata_type(&sub, var.value);

		data = EdbusData::from_variant(var);
		return;
	}

	if(dtype == DBUS_TYPE_STRUCT) {
		EdbusList s = EdbusList::create_struct();
		EdbusData val;
		DBusMessageIter struct_it;

		dbus_message_iter_recurse(iter, &struct_it);
		while(dbus_message_iter_get_arg_type(&struct_it) != DBUS_TYPE_INVALID) {
			from_dbus_iter_to_edbusdata_type(&struct_it, val);	
			s.append(val);

			dbus_message_iter_next(&struct_it);
		}

		data = EdbusData::from_struct(s);
		return;
	}

	E_ASSERT(0 && "Got some unknown type from D-Bus ???");
}


EdbusMessage::EdbusMessage() : dm(NULL) {
}

EdbusMessage::EdbusMessage(DBusMessage* msg) : dm(NULL) {
	from_dbus_message(msg);
}

EdbusMessage::~EdbusMessage() {
	if(!dm)
		return;

	clear_all();
	delete dm;
}

#define CREATE_OR_CLEAR(m)                      \
do {                                            \
	if(!m) {                                    \
		m = new EdbusMessageImpl;               \
		m->msg = NULL;                          \
	} else {                                    \
		/* destroy previously create message */ \
		clear_all();                            \
	}                                           \
} while(0)


void EdbusMessage::from_dbus_message(DBusMessage* m) {
	CREATE_OR_CLEAR(dm);

	dm->msg = m;
	/* increase counter or libdbus will scream with assertion */
	dm->msg = dbus_message_ref(dm->msg);

	DBusMessageIter iter;
	if(!dbus_message_iter_init(dm->msg, &iter)) {
		puts("Can't init iterator");
		return;
	}

	int t;
	while((t = dbus_message_iter_get_arg_type(&iter)) != DBUS_TYPE_INVALID) {
		EdbusData d;

		from_dbus_iter_to_edbusdata_type(&iter, d);
		msg_content.push_back(d);

		dbus_message_iter_next(&iter);
	}
}

DBusMessage* EdbusMessage::to_dbus_message(void) const {
	E_ASSERT(dm != NULL);
	E_ASSERT(dm->msg != NULL);

	DBusMessageIter iter;
	dbus_message_iter_init_append(dm->msg, &iter);

	EdbusMessage::const_iterator it = begin(), it_end = end();
	while(it != it_end) {
		to_dbus_iter_from_edbusdata_type(&iter, *it);
		++it;
	}

	return dm->msg;
}

void EdbusMessage::create_signal(const char* path, const char* interface, const char* name) {
	CREATE_OR_CLEAR(dm);
	dm->msg = dbus_message_new_signal(path, interface, name);
}

void EdbusMessage::create_method_call(const char* service, const char* path, const char* interface, const char* method) {
	CREATE_OR_CLEAR(dm);
	dm->msg = dbus_message_new_method_call(service, path, interface, method);
}

void EdbusMessage::create_reply(const EdbusMessage& replying_to) {
	CREATE_OR_CLEAR(dm);
	dm->msg = dbus_message_new_method_return(replying_to.dm->msg);
}

void EdbusMessage::create_error_reply(const EdbusMessage& replying_to, const char* errmsg) {
	CREATE_OR_CLEAR(dm);
	dm->msg = dbus_message_new_error(replying_to.dm->msg, DBUS_ERROR_FAILED, errmsg);
}

void EdbusMessage::clear_all(void) {
	if(!dm)
		return;

	if(dm->msg != NULL) {
		dbus_message_unref(dm->msg);
		dm->msg = NULL;
	}

	msg_content.clear();
}

bool EdbusMessage::is_signal(void) {
	if(!dm || !dm->msg)
		return false;
	return dbus_message_is_signal(dm->msg, interface(), member());
}

bool EdbusMessage::is_method_call(void) {
	if(!dm || !dm->msg)
		return false;
	return dbus_message_is_method_call(dm->msg, interface(), member());
}

bool EdbusMessage::is_error_reply(const char* errmsg) {
	if(!dm)
		return false;
	return dbus_message_is_error(dm->msg, errmsg);
}

void EdbusMessage::path(const char* np) {
	if(!dm || !dm->msg)
		return;

	dbus_message_set_path(dm->msg, np);
}

const char* EdbusMessage::path(void) const {
	if(!dm || !dm->msg)
		return NULL;

	return dbus_message_get_path(dm->msg);
}

void EdbusMessage::interface(const char* ni) {
	if(!dm || !dm->msg)
		return;

	dbus_message_set_interface(dm->msg, ni);
}

const char* EdbusMessage::interface(void) const {
	if(!dm || !dm->msg)
		return NULL;

	return dbus_message_get_interface(dm->msg);
}

void EdbusMessage::destination(const char* nd) {
	if(!dm || !dm->msg)
		return;

	dbus_message_set_destination(dm->msg, nd);
}

const char* EdbusMessage::destination(void) const {
	if(!dm || !dm->msg)
		return NULL;

	return dbus_message_get_destination(dm->msg);
}

void EdbusMessage::member(const char* nm) {
	if(!dm || !dm->msg)
		return;

	dbus_message_set_member(dm->msg, nm);
}

const char* EdbusMessage::member(void) const {
	if(!dm || !dm->msg)
		return NULL;

	return dbus_message_get_member(dm->msg);
}

void EdbusMessage::sender(const char* ns) {
	if(!dm || !dm->msg)
		return;

	dbus_message_set_sender(dm->msg, ns);
}

const char* EdbusMessage::sender(void) const {
	if(!dm || !dm->msg)
		return NULL;

	return dbus_message_get_sender(dm->msg);
}

const char* EdbusMessage::signature(void) const {
	if(!dm || !dm->msg)
		return NULL;

	return dbus_message_get_signature(dm->msg);
}

EDELIB_NS_END
