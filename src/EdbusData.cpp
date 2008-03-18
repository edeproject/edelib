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

#include <string.h>
#include <stdlib.h>

#include <edelib/Debug.h>
#include <edelib/EdbusData.h>
#include <edelib/EdbusDict.h>
#include <edelib/EdbusList.h>

EDELIB_NS_BEGIN

struct EdbusDataPrivate {
	uint32_t      refs;
	EdbusDataType type;

	union {
		bool     v_bool;
		byte_t   v_byte;
		int16_t  v_int16;
		uint16_t v_uint16;
		int32_t  v_int32;
		uint32_t v_uint32;
		int64_t  v_int64;
		uint64_t v_uint64;
		double   v_double;
		void*    v_pointer;
	} value;
};

EdbusData::EdbusData() {
	impl = new EdbusDataPrivate;
	impl->refs = 1;
	impl->type = EDBUS_TYPE_INVALID;
}

EdbusData::EdbusData(byte_t val) {
	impl = new EdbusDataPrivate;
	impl->refs = 1;
	impl->type = EDBUS_TYPE_BYTE;
	impl->value.v_byte = val;
}

EdbusData::EdbusData(bool val) {
	impl = new EdbusDataPrivate;
	impl->refs = 1;
	impl->type = EDBUS_TYPE_BOOL;
	impl->value.v_bool = val;
}

EdbusData::EdbusData(int16_t val) {
	impl = new EdbusDataPrivate;
	impl->refs = 1;
	impl->type = EDBUS_TYPE_INT16;
	impl->value.v_int16 = val;
}

EdbusData::EdbusData(uint16_t val) {
	impl = new EdbusDataPrivate;
	impl->refs = 1;
	impl->type = EDBUS_TYPE_UINT16;
	impl->value.v_uint16 = val;
}

EdbusData::EdbusData(int32_t val) {
	impl = new EdbusDataPrivate;
	impl->refs = 1;
	impl->type = EDBUS_TYPE_INT32;
	impl->value.v_int32 = val;
}

EdbusData::EdbusData(uint32_t val) {
	impl = new EdbusDataPrivate;
	impl->refs = 1;
	impl->type = EDBUS_TYPE_UINT32;
	impl->value.v_uint32 = val;
}

EdbusData::EdbusData(int64_t val) {
	impl = new EdbusDataPrivate;
	impl->refs = 1;
	impl->type = EDBUS_TYPE_INT64;
	impl->value.v_int64 = val;
}

EdbusData::EdbusData(uint64_t val) {
	impl = new EdbusDataPrivate;
	impl->refs = 1;
	impl->type = EDBUS_TYPE_UINT64;
	impl->value.v_uint64 = val;
}

EdbusData::EdbusData(double val) {
	impl = new EdbusDataPrivate;
	impl->refs = 1;
	impl->type = EDBUS_TYPE_DOUBLE;
	impl->value.v_double = val;
}

EdbusData::EdbusData(const char* val) {
	impl = new EdbusDataPrivate;
	impl->refs = 1;
	impl->type = EDBUS_TYPE_STRING;
	impl->value.v_pointer = strdup(val);
}

EdbusData::EdbusData(const EdbusObjectPath& val) {
	impl = new EdbusDataPrivate;
	impl->refs = 1;
	impl->type = EDBUS_TYPE_OBJECT_PATH;
	impl->value.v_pointer = strdup(val.path());
}

EdbusData::EdbusData(const EdbusVariant& val) {
	impl = new EdbusDataPrivate;
	impl->refs = 1;
	impl->type = EDBUS_TYPE_VARIANT;

	/* make a shallow copy */
	impl->value.v_pointer = new EdbusVariant(val);
}

EdbusData::EdbusData(const EdbusDict& val) {
	impl = new EdbusDataPrivate;
	impl->refs = 1;
	impl->type = EDBUS_TYPE_DICT;

	/* make a shallow copy */
	impl->value.v_pointer = new EdbusDict(val);
}

EdbusData::EdbusData(const EdbusList& val) {
	impl = new EdbusDataPrivate;
	impl->refs = 1;

	if(val.list_is_array())
		impl->type = EDBUS_TYPE_ARRAY;
	else
		impl->type = EDBUS_TYPE_STRUCT;

	/* make a shallow copy */
	impl->value.v_pointer = new EdbusList(val);
}

EdbusData::EdbusData(const EdbusData& other) {
	if(this == &other)
		return;

	impl = other.impl;
	other.impl->refs++;
}

EdbusData::~EdbusData() {
	if(!impl)
		return;

	impl->refs--;

	if(impl->refs == 0)
		dispose();
}

void EdbusData::dispose(void) {
	if(impl->type == EDBUS_TYPE_STRING || impl->type == EDBUS_TYPE_OBJECT_PATH)
		free(impl->value.v_pointer);
	else if(impl->type == EDBUS_TYPE_VARIANT) {
		/* variant uses new operator */
		EdbusVariant* v = (EdbusVariant*)impl->value.v_pointer;
		delete v;
	} else if(impl->type == EDBUS_TYPE_DICT) {
		/* dict uses new operator */
		EdbusDict* d = (EdbusDict*)impl->value.v_pointer;
		delete d;
	} else if(impl->type == EDBUS_TYPE_ARRAY || impl->type == EDBUS_TYPE_STRUCT) {
		EdbusList* l = (EdbusList*)impl->value.v_pointer;
		delete l;
	}

	delete impl;
	impl = 0;
}

EdbusData& EdbusData::operator=(const EdbusData& other) {
	other.impl->refs++;
	impl->refs--;

	if(impl->refs == 0)
		dispose();

	impl = other.impl;
	return *this;
}

EdbusDataType EdbusData::type() const {
	return impl->type;
}

byte_t EdbusData::to_byte(void) const {
	E_ASSERT(is_byte() == true);
	return impl->value.v_byte;
}

bool EdbusData::to_bool(void) const {
	E_ASSERT(is_bool() == true);
	return impl->value.v_bool;
}

int16_t EdbusData::to_int16(void) const {
	E_ASSERT(is_int16() == true);
	return impl->value.v_int16;
}

uint16_t EdbusData::to_uint16(void) const {
	E_ASSERT(is_uint16() == true);
	return impl->value.v_uint16;
}

int32_t EdbusData::to_int32(void) const {
	E_ASSERT(is_int32() == true);
	return impl->value.v_int32;
}

uint32_t EdbusData::to_uint32(void) const {
	E_ASSERT(is_uint32() == true);
	return impl->value.v_uint32;
}

int64_t EdbusData::to_int64(void) const {
	E_ASSERT(is_int64() == true);
	return impl->value.v_int64;
}

uint64_t EdbusData::to_uint64(void) const {
	E_ASSERT(is_uint64() == true);
	return impl->value.v_uint64;
}

double EdbusData::to_double(void) const {
	E_ASSERT(is_uint64() == true);
	return impl->value.v_double;
}

const char* EdbusData::to_string(void) const {
	E_ASSERT(is_string() == true);
	return (const char*)impl->value.v_pointer;
}

EdbusObjectPath EdbusData::to_object_path(void) const {
	E_ASSERT(is_object_path() == true);
	return EdbusObjectPath((const char*)impl->value.v_pointer);
}

EdbusVariant EdbusData::to_variant(void) const {
	E_ASSERT(is_variant() == true);
	/* copy variant (a shallow copy) */
	return EdbusVariant((*(EdbusVariant*)impl->value.v_pointer));
}

EdbusDict EdbusData::to_dict(void) const {
	E_ASSERT(is_dict() == true);
	/* copy dict (a shallow copy) */
	return EdbusDict((*(EdbusDict*)impl->value.v_pointer));
}

EdbusList EdbusData::to_array(void) const {
	E_ASSERT(is_array() == true);
	/* copy dict (a shallow copy) */
	return EdbusList((*(EdbusList*)impl->value.v_pointer));
}

EdbusList EdbusData::to_struct(void) const {
	E_ASSERT(is_struct() == true);
	/* copy dict (a shallow copy) */
	return EdbusList((*(EdbusList*)impl->value.v_pointer));
}

bool EdbusData::operator==(const EdbusData& other) const {
	if(&other == this)
		return true;

	if(type() != other.type())
		return false;

	switch(type()) {
		case EDBUS_TYPE_INVALID:
			return true;
		case EDBUS_TYPE_BYTE:
			return impl->value.v_byte == other.impl->value.v_byte;
		case EDBUS_TYPE_BOOL:
			return impl->value.v_bool == other.impl->value.v_bool;
		case EDBUS_TYPE_INT16:
			return impl->value.v_int16 == other.impl->value.v_int16;
		case EDBUS_TYPE_UINT16:
			return impl->value.v_uint16 == other.impl->value.v_uint16;
		case EDBUS_TYPE_INT32:
			return impl->value.v_int32 == other.impl->value.v_int32;
		case EDBUS_TYPE_UINT32:
			return impl->value.v_uint32 == other.impl->value.v_uint32;
		case EDBUS_TYPE_INT64:
			return impl->value.v_int64 == other.impl->value.v_int64;
		case EDBUS_TYPE_UINT64:
			return impl->value.v_uint64 == other.impl->value.v_uint64;
		case EDBUS_TYPE_DOUBLE:
			return impl->value.v_double == other.impl->value.v_double;
		case EDBUS_TYPE_STRING:
			/* TODO: use edelib::String here */
			if(impl->value.v_pointer && other.impl->value.v_pointer) {
				const char* v1 = (const char*)impl->value.v_pointer;
				const char* v2 = (const char*)other.impl->value.v_pointer;
				return (strcmp(v1, v2) == 0);
			} else
				return false;

		case EDBUS_TYPE_OBJECT_PATH: {
			EdbusObjectPath* v1 = (EdbusObjectPath*)impl->value.v_pointer;
			EdbusObjectPath* v2 = (EdbusObjectPath*)other.impl->value.v_pointer;
			return *v1 == *v2;
		}

		case EDBUS_TYPE_ARRAY:
		case EDBUS_TYPE_STRUCT: {
			EdbusList* v1 = (EdbusList*)impl->value.v_pointer;
			EdbusList* v2 = (EdbusList*)other.impl->value.v_pointer;
			return *v1 == *v2;
		}

		case EDBUS_TYPE_DICT: {
			EdbusDict* d1 = (EdbusDict*)impl->value.v_pointer;
			EdbusDict* d2 = (EdbusDict*)impl->value.v_pointer;
			return *d1 == *d2;
	  	}

		case EDBUS_TYPE_VARIANT: {
			EdbusVariant* v1 = (EdbusVariant*)impl->value.v_pointer;
			EdbusVariant* v2 = (EdbusVariant*)other.impl->value.v_pointer;
			return v1->value == v2->value;
		}
	}

	return false;
}

EDELIB_NS_END
