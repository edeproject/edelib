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

#ifndef __EDBUSDATA_H__
#define __EDBUSDATA_H__

#include "econfig.h"

#include <inttypes.h>
#include "EdbusObjectPath.h"

typedef char byte_t;

EDELIB_NS_BEGIN

/**
 * \ingroup dbus
 * \enum EdbusDataType
 * \brief Type of data current EdbusData object holds
 */
enum EdbusDataType {
	EDBUS_TYPE_INVALID,        ///< Invalid type (empty object)
	EDBUS_TYPE_BYTE,           ///< byte
	EDBUS_TYPE_BOOL,           ///< bool
	EDBUS_TYPE_INT16,          ///< short
	EDBUS_TYPE_UINT16,         ///< unsigned short
	EDBUS_TYPE_INT32,          ///< integer
	EDBUS_TYPE_UINT32,         ///< unsigned integer
	EDBUS_TYPE_INT64,          ///< long long
	EDBUS_TYPE_UINT64,         ///< unsigned long long
	EDBUS_TYPE_DOUBLE,         ///< double
	EDBUS_TYPE_STRING,         ///< string
	EDBUS_TYPE_OBJECT_PATH,    ///< EdbusObjectPath type
	EDBUS_TYPE_ARRAY,          ///< array
	EDBUS_TYPE_STRUCT,         ///< structure
	EDBUS_TYPE_DICT,           ///< dictionary
	EDBUS_TYPE_VARIANT         ///< variant
};

#ifndef SKIP_DOCS
struct EdbusDataPrivate;
#endif

class  EdbusDict;
class  EdbusList;
struct EdbusVariant;

/**
 * \ingroup dbus
 * \class EdbusData
 * \brief Class for representing D-Bus data types
 *
 * This class is variant-like class; this means it can hold
 * any of already predefined types (see EdbusDataType for the list)
 * that D-Bus protocol understainds.
 *
 * Class use implicit sharing so copying EdbusData objects is cheap
 * operation that does not require content copying.
 *
 * Since EdbusData object can hold only one type at the time (I'm assuming
 * arrays or structs are types too), when you decide to fetch a currently
 * holding value, <em>make sure</em> you call correct function (e.g. for 
 * bool to_bool() or for double to_double()) or assertion will be triggered.
 *
 * The best way to check what current type is, is to use one of is_XYZ members
 * (e.g. for bool is_bool() or for double is_double()).
 *
 * When data is assigned to EdbusData, it can implicitly resolve data type
 * (e.g. strings, integers or EdbusObjectPath), but in case of similar types, 
 * things can a bit strange at first look. To better explain this, let say
 * you assign <em>5</em> as value like:
 * \code
 *   EdbusData d = 5;
 * \endcode
 *
 * What is <em>d</em> type? byte_t, int16_t, uint16_t, int32_t, uint32_t, int64_t and
 * uint64_t can holds this value; by default, a compiler will use constructor
 * with int32_t and <em>d</em> will be that type. This let-things-done-by-compiler
 * stuff can introduce a strange bugs, so static <em>from_</em> members are given to
 * make life easier for compiler, like:
 * \code
 *   EdbusData d = EdbusData::from_int32(5);
 *   // or to force it inot uint16_t
 *   EdbusData d = EdbusData::from_uint16(5);
 * \endcode
 *
 * \note D-Bus signature for above types is different and sending int32_t to the service
 * that expects uint16_t will probably result rejecting whole message!
 */
class EdbusData {
	private:
		EdbusDataPrivate* impl;
		void dispose(void);

	public:
		/**
		 * Construct object with invalid type
		 */
		EdbusData();

		/**
		 * Construct object with byte (char) type
		 */
		EdbusData(byte_t val);

		/**
		 * Construct object with bool type
		 */
		EdbusData(bool val);

		/**
		 * Construct object with int16 (short) type
		 */
		EdbusData(int16_t val);

		/**
		 * Construct object with uint16 (unsigned short) type
		 */
		EdbusData(uint16_t val);

		/**
		 * Construct object with int32 (int) type
		 */
		EdbusData(int32_t val);

		/**
		 * Construct object with uint32 (unsigned int) type
		 */
		EdbusData(uint32_t val);

		/**
		 * Construct object with int64 (long long) type
		 */
		EdbusData(int64_t val);

		/**
		 * Construct object with uint64 (unsigned long long) type
		 */
		EdbusData(uint64_t val);

		/**
		 * Construct object with double type
		 */
		EdbusData(double val);

		/**
		 * Construct object with string type
		 */
		EdbusData(const char* val);

		/**
		 * Construct object with EdbusObjectPath type
		 */
		EdbusData(const EdbusObjectPath& val);

		/**
		 * Construct object with EdbusVariant type
		 */
		EdbusData(const EdbusVariant& val);

		/**
		 * Construct object with EdbusDict type
		 */
		EdbusData(const EdbusDict& val);

		/**
		 * Construct object with EdbusList type. Correct type will
		 * be deduced from EdbusList::list_is_array() and EdbusList::list_is_struct() members
		 */
		EdbusData(const EdbusList& val);

		/**
		 * Construct object with already constructed object.
		 * Essential for containers
		 */
		EdbusData(const EdbusData& other);

		/**
		 * Destructs object cleaning allocated data
		 */
		~EdbusData();

		/**
		 * Returns a current holding type
		 */
		EdbusDataType type(void) const;

		/**
		 * Returns a byte value if it holds
		 */
		byte_t to_byte(void) const;

		/**
		 * Returns a char value if it holds
		 */
		char to_char(void) const { return to_byte(); }

		/**
		 * Returns a bool value if it holds
		 */
		bool to_bool(void) const;

		/**
		 * Returns a int16 value if it holds
		 */
		int16_t to_int16(void) const;

		/**
		 * Returns a uint16 value if it holds
		 */
		uint16_t to_uint16(void) const;

		/**
		 * Returns a int32 value if it holds
		 */
		int32_t to_int32(void) const;

		/**
		 * Returns a uint32 value if it holds
		 */
		uint32_t to_uint32(void) const;

		/**
		 * Returns a int64 value if it holds
		 */
		int64_t  to_int64(void) const;

		/**
		 * Returns a uint64 value if it holds
		 */
		uint64_t to_uint64(void) const;

		/**
		 * Returns a double value if it holds
		 */
		double to_double(void) const;

		/**
		 * Returns a string value if it holds
		 */
		const char* to_string(void) const;

		/**
		 * Returns a EdbusObjectPath value if it holds
		 */
		EdbusObjectPath to_object_path(void) const;

		/**
		 * Returns a EdbusVariant value if it holds
		 */
		EdbusVariant to_variant(void) const;

		/**
		 * Returns a EdbusDict value if it holds
		 */
		EdbusDict to_dict(void) const;

		/**
		 * Returns a EdbusList object as array
		 */
		EdbusList to_array(void) const;

		/**
		 * Returns a EdbusList object as struct
		 */
		EdbusList to_struct(void) const;

		/**
		 * Assign existing object
		 */
		EdbusData& operator=(const EdbusData& other);

		/**
		 * Check if two objects are the same checking their types
		 * and comparing their values if they are same type
		 */
		bool operator==(const EdbusData& other) const;

		/**
		 * Check if two objects are not equal
		 */
		bool operator!=(const EdbusData& other) const { return !operator==(other); }

		/**
		 * Returns true if object currently holds a value of valid type
		 */
		bool is_valid(void) const { return type() != EDBUS_TYPE_INVALID; }

		/**
		 * Returns true if object currently holds a byte value
		 */
		bool is_byte(void) const { return type() == EDBUS_TYPE_BYTE; }

		/**
		 * Returns true if object currently holds a char value
		 */
		bool is_char(void) const { return is_byte(); }

		/**
		 * Returns true if object currently holds a bool value
		 */
		bool is_bool(void) const { return type() == EDBUS_TYPE_BOOL; }

		/**
		 * Returns true if object currently holds a int16 value
		 */
		bool is_int16(void) const { return type() == EDBUS_TYPE_INT16; }

		/**
		 * Returns true if object currently holds a uint16 value
		 */
		bool is_uint16(void) const { return type() == EDBUS_TYPE_UINT16; }

		/**
		 * Returns true if object currently holds a int32 value
		 */
		bool is_int32(void) const { return type() == EDBUS_TYPE_INT32; }

		/**
		 * Returns true if object currently holds a uint32 value
		 */
		bool is_uint32(void) const { return type() == EDBUS_TYPE_UINT32; }

		/**
		 * Returns true if object currently holds a int64 value
		 */
		bool is_int64(void) const { return type() == EDBUS_TYPE_INT64; }

		/**
		 * Returns true if object currently holds a uint64 value
		 */
		bool is_uint64(void) const { return type() == EDBUS_TYPE_UINT64; }

		/**
		 * Returns true if object currently holds a double value
		 */
		bool is_double(void) const { return type() == EDBUS_TYPE_DOUBLE; }

		/**
		 * Returns true if object currently holds a string value
		 */
		bool is_string(void) const { return type() == EDBUS_TYPE_STRING; }

		/**
		 * Returns true if object currently holds a EdbusObjectPath value
		 */
		bool is_object_path(void) const { return type() == EDBUS_TYPE_OBJECT_PATH; }

		/**
		 * Returns true if object currently holds a EdbusList as array value
		 */
		bool is_array(void) const { return type() == EDBUS_TYPE_ARRAY; }

		/**
		 * Returns true if object currently holds a EdbusList as struct value
		 */
		bool is_struct(void) const { return type() == EDBUS_TYPE_STRUCT; }

		/**
		 * Returns true if object currently holds a EdbusVariant value
		 */
		bool is_variant(void) const { return type() == EDBUS_TYPE_VARIANT; }

		/**
		 * Returns true if object currently holds a EdbusDict value
		 */
		bool is_dict(void) const { return type() == EDBUS_TYPE_DICT; }

		/**
		 * Returns true if given object is basic type.
		 * Basic types (to D-Bus) are all EdbusDataType types except
		 * array, structure, dict and variant
		 */
		static bool basic_type(const EdbusData& val) {
			return ((val.type() != EDBUS_TYPE_ARRAY) && (val.type() != EDBUS_TYPE_STRUCT) &&
				(val.type() != EDBUS_TYPE_VARIANT) && (val.type() != EDBUS_TYPE_DICT));
		}

		/**
		 * Creates object of invalid type
		 */
		static EdbusData from_invalid(void) { return EdbusData(); }

		/**
		 * Creates object with byte value
		 */
		static EdbusData from_byte(byte_t val) { return EdbusData((byte_t)val); }

		/**
		 * Creates object with char value
		 */
		static EdbusData from_char(char val) { return from_byte(val); }

		/**
		 * Creates object with bool value
		 */
		static EdbusData from_bool(bool val) { return EdbusData((bool)val); }

		/**
		 * Creates object with int16 value
		 */
		static EdbusData from_int16(int16_t val) { return EdbusData((int16_t)val); }

		/**
		 * Creates object with uint16 value
		 */
		static EdbusData from_uint16(uint16_t val) { return EdbusData((uint16_t)val); }

		/**
		 * Creates object with int32 value
		 */
		static EdbusData from_int32(int32_t val) { return EdbusData((int32_t)val); }

		/**
		 * Creates object with uint32 value
		 */
		static EdbusData from_uint32(uint32_t val) { return EdbusData((uint32_t)val); }

		/**
		 * Creates object with int64 value
		 */
		static EdbusData from_int64(int64_t val) { return EdbusData((int64_t)val); }

		/**
		 * Creates object with uint64 value
		 */
		static EdbusData from_uint64(uint64_t val) { return EdbusData((uint64_t)val); }

		/**
		 * Creates object with double value
		 */
		static EdbusData from_double(double val) { return EdbusData((double)val); }

		/**
		 * Creates object with string value
		 */
		static EdbusData from_string(const char* val) { return EdbusData((const char*)val); }

		/**
		 * Creates object with EdbusObjectPath value
		 */
		static EdbusData from_object_path(const EdbusObjectPath& val) { return EdbusData(val); }

		/**
		 * Creates object with EdbusVariant value
		 */
		static EdbusData from_variant(const EdbusVariant& val) { return EdbusData(val); }

		/**
		 * Creates object with EdbusDict value
		 */
		static EdbusData from_dict(const EdbusDict& val) { return EdbusData(val); }

		/**
		 * Creates array object. EdbusData constructor will handle EdbusList type
		 */
		static EdbusData from_array(const EdbusList& val) { return EdbusData(val); }

		/**
		 * Creates struct object. EdbusData constructor will handle EdbusList type
		 */
		static EdbusData from_struct(const EdbusList& val) { return EdbusData(val); }
};

/**
 * \ingroup dbus
 * \class EdbusVariant
 * \brief Represents D-Bus variant
 */
struct EdbusVariant {
	/** variant value */
	EdbusData value;
};

EDELIB_NS_END

#endif
