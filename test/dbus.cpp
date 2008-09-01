#include <string.h>

#include <edelib/EdbusDict.h>
#include <edelib/EdbusObjectPath.h>
#include <edelib/EdbusList.h>
#include <edelib/EdbusDict.h>

#include "UnitTest.h"

#define CCHARP(str)           ((const char*)str)
#define STR_EQUAL(str1, str2) (strcmp(CCHARP(str1), CCHARP(str2)) == 0)

EDELIB_NS_USE;

UT_FUNC(TestEdbusData, "Test EdbusData")
{
	EdbusData data = EdbusData::from_invalid();
	UT_VERIFY_NOT_EQUAL( data.is_valid(), true );

	data = EdbusData::from_byte(1);
	UT_VERIFY( data.is_byte() == true );
	UT_VERIFY( data.is_char() == true );
	UT_VERIFY( data.to_byte() == 1 );

	data = EdbusData::from_char('c');
	UT_VERIFY( data.is_char() == true );
	UT_VERIFY( data.is_byte() == true );
	UT_VERIFY( data.to_char() == 'c' );

	data = EdbusData::from_bool(true);
	UT_VERIFY( data.is_bool() == true );
	UT_VERIFY( data.to_bool() == true );

	data = EdbusData::from_int16(11);
	UT_VERIFY( data.is_int16() == true );
	UT_VERIFY( data.to_int16() == 11 );

	data = EdbusData::from_uint16(11);
	UT_VERIFY( data.is_uint16() == true );
	UT_VERIFY( data.to_uint16() == 11 );

	data = EdbusData::from_int32(11);
	UT_VERIFY( data.is_int32() == true );
	UT_VERIFY( data.to_int32() == 11 );

	data = EdbusData::from_uint32(11);
	UT_VERIFY( data.is_uint32() == true );
	UT_VERIFY( data.to_uint32() == 11 );

	data = EdbusData::from_int64(11);
	UT_VERIFY( data.is_int64() == true );
	UT_VERIFY( data.to_int64() == 11 );

	data = EdbusData::from_uint64(11);
	UT_VERIFY( data.is_uint64() == true );
	UT_VERIFY( data.to_uint64() == 11 );

	data = EdbusData::from_double(0.1);
	UT_VERIFY( data.is_double() == true );
	UT_VERIFY( data.to_double() == 0.1 );

	data = EdbusData::from_string("foo");
	UT_VERIFY( data.is_string() == true );
	UT_VERIFY( STR_EQUAL(data.to_string(), "foo") );

	EdbusData data2 = data;
	UT_VERIFY( data == data2 );

	data2 = EdbusData::from_int32(10);
	UT_VERIFY( data.is_string() == true );
	UT_VERIFY( data2.is_int32() == true );
	UT_VERIFY( data != data2 );

	EdbusObjectPath op;
	op << "org" << "example" << "foo";
	data = EdbusData::from_object_path(op);
	UT_VERIFY( data.is_object_path() == true );
	UT_VERIFY( STR_EQUAL(data.to_object_path().path(), "/org/example/foo") );

	EdbusList arr(true);
	arr << 1 << 2 << 3 << 4;
	data = EdbusData::from_array(arr);
	UT_VERIFY( data.is_array() == true );
	UT_VERIFY( data.to_array() == arr );

	EdbusList s(false);
	s << 1 << 'c' << "foo";
	data = EdbusData::from_struct(s);
	UT_VERIFY( data.is_struct() == true );
	UT_VERIFY( data.to_struct() == s );

	EdbusDict d;
	d.append("key1", "value1");
	d.append("key2", "value2");
	data = EdbusData::from_dict(d);
	UT_VERIFY( data.is_dict() == true );
	UT_VERIFY( data.to_dict() == d );

	EdbusVariant v;
	v.value = data2;
	data = EdbusData::from_variant(v);
	UT_VERIFY( data.is_variant() == true );
	UT_VERIFY( data.to_variant().value == v.value );
}

UT_FUNC(TestEdbusList, "Test EdbusList")
{
	// first as array
	EdbusList arr = EdbusList::create_array();
	UT_VERIFY( arr.list_is_array() == true );

	arr << 1 << 2 << 3 << 4 << 5;
	UT_VERIFY( arr.value_type() == EDBUS_TYPE_INT32 );
	UT_VERIFY( arr.value_type_is_container() == false );
	UT_VERIFY( arr.size() == 5 );

	EdbusList::const_iterator it = arr.begin(), it_end = arr.end();

	UT_VERIFY( (*it).to_int32() == 1 ); ++it;
	UT_VERIFY( (*it).to_int32() == 2 ); ++it;
	UT_VERIFY( (*it).to_int32() == 3 ); ++it;
	UT_VERIFY( (*it).to_int32() == 4 ); ++it;
	UT_VERIFY( (*it).to_int32() == 5 );

	arr.remove(5);
	UT_VERIFY( arr.size() == 4 );

	// check copying and sharing state
	EdbusList arr2 = arr;
	UT_VERIFY( arr2 == arr );
	arr2 << 10;
	UT_VERIFY( arr2 != arr );
	UT_VERIFY( arr2.size() == 5 );
	UT_VERIFY( arr.size() == 4 );

	// reject different types
	arr << 'c';
	arr << "foo";
	arr << EdbusData::from_uint32(12);
	arr << EdbusData::from_int16(12);
	UT_VERIFY( arr.size() == 4 );

	// remove all elements with the same value
	arr << 4 << 5 << 5 << 5;
	UT_VERIFY( arr.size() == 8 );
	arr.remove_all(4);
	arr.remove_all(5);
	UT_VERIFY( arr.size() == 3 );

	arr.clear();
	UT_VERIFY( arr.size() == 0 );
	UT_VERIFY( arr != arr2 );

	// now as struct
	arr = EdbusList::create_struct();
	UT_VERIFY( arr.list_is_struct() == true );

	arr << 1 << 2 << 'c' << EdbusData::from_int16(10) << "foobar";
	UT_VERIFY( arr.size() == 5 );
	UT_VERIFY( arr != arr2 );

	EdbusList arr3 = arr;
	UT_VERIFY( arr == arr3 );
	arr3.clear();
	UT_VERIFY( arr != arr3 );

	it = arr.begin(), it_end = arr.end();
	UT_VERIFY( (*it).to_int32() == 1 ); ++it;
	UT_VERIFY( (*it).to_int32() == 2 ); ++it;
	UT_VERIFY( (*it).to_char() == 'c' ); ++it;
	UT_VERIFY( (*it).to_int16() == 10 ); ++it;
	UT_VERIFY( STR_EQUAL((*it).to_string(), "foobar") );
}

UT_FUNC(TestEdbusDict, "Test EdbusDict")
{
	EdbusDict dict;
	dict.append("key1", "value1");
	dict.append("key2", "value2");
	dict.append("key3", "value3");

	UT_VERIFY( dict.size() == 3 );

	// do not append duplicate keys, just append their values
	dict.append("key1", "foo");
	UT_VERIFY( dict.size() == 3 );
	UT_VERIFY( dict.key_type() == EDBUS_TYPE_STRING );
	UT_VERIFY( dict.value_type() == EDBUS_TYPE_STRING );
	UT_VERIFY( dict.value_type_is_container() == false );
}
