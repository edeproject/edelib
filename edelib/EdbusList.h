#ifndef __EDBUSLIST_H__
#define __EDBUSLIST_H__

#include "EdbusData.h"
#include "EdbusContainer.h"

/**
 * \class EdbusList
 * \brief A class representing D-Bus struct and array
 *
 * EdbusList is used to implement D-Bus struct and array concept. Althought EdbusList
 * is much more list-like container (indeed, it uses list for storage), accessing
 * elements is done via iterators which, at some point, is similar to accessing elements
 * to ordinary arrays via pointers (if EdbusList is in <em>array</em> mode).
 *
 * At first look EdbusList sees array and struct at the same way. This is true, at some
 * point, but there is major difference (as in C++ language): arrays can hold only elements
 * of the same type, but structs can hold elements of any type. 
 *
 * This sounds little bit strange, especially knowing that EdbusList element is EdbusData type,
 * but if you look at EdbusData more as proxy for D-Bus types than as concrete type, things will
 * be little bit clearer. This will be much better explained in few examples below.
 *
 * So, from previous text, EdbusList object can be either array-like or struct-like. This is done
 * via constructor.
 *
 * When EdbusList object behaves as array, it will assure all elements are the same type so if
 * you try to add via append() element of different type, it will be ignored.
 *
 * \todo This should be probably an assertion
 *
 * When EdbusList object behaves as struct, elements of any type can be added.
 *
 * The best way to create EdbusList array object is to use EdbusList::create_array(); it will call 
 * EdbusList constructor with correct parameters. The same applies for struct, use EdbusList::create_struct().
 *
 * Let say you want to prepare some array for sending via bus. This is the way:
 * \code
 *   int numbers[] = {1, 2, 3};
 *   EdbusList array = EdbusList::create_array();
 *   array << EdbusData::from_int32(numbers[0]) << EdbusData::from_int32(numbers[1]) << EdbusData::from_int32(numbers[2]);
 * \endcode
 *
 * Since operator<<() is shorcut for append(), you can use append() instead too.
 *
 * Now, let say you want to send some struct via bus. Sample:
 * \code
 *    // somewhere outside function
 *    struct some_struct {
 *      int number;
 *      char character;
 *    };
 *
 *    // in your function
 *    some_struct v;
 *    v.number = 3;
 *    v.character = 'c';
 *
 *    EdbusList st = EdbusList::create_struct();
 *    st << EdbusData::from_int32(v.number);
 *    st << EdbusData::from_char(v.character);
 * \endcode
 *
 * If EdbusList object is created as array and you add first element, all further elements should be
 * the same type as first element or they will not be added, like:
 * \code
 *   EdbusList array = EdbusList::create_array();
 *   array << EdbusData::from_bool(false);
 *   array << EdbusData::from_int32(45);     // different type than first element, ignored
 *   array << EdbusData::from_bool(true);   // the same type as first element, ok
 * \endcode
 *
 * Some things can look like correct C++ statement, but due D-Bus signatures they are not, like:
 *
 * \code
 *   EdbusList array = EdbusList::create_array();
 *   array << EdbusData::from_bool(true);
 *
 *   // invalid; singatures for boolean and int32 types are different
 *   // no matter if this is a valid from C++ view
 *   array << EdbusData::from_int32(true);
 * \endcode
 *
 * For more details, \see EdbusData
 *
 * On other hand, if elements of array is EdbusVariant typs, array <em>can</em> hold different types
 * due EdbusVariant nature. You can look at this as case when you have ordinary C++ array of variant-like objects.
 *
 * EdbusList uses implicit sharing.
 */
class EdbusList : public EdbusContainer<EdbusData> {
	private:
		bool array_mode;
		EdbusList();

	public:
		/**
	 	 * Declares EdbusList iterator
	 	 */
		typedef EdbusContainer<EdbusData>::const_iterator const_iterator;

		/**
		 * Create empty container
		 * \param a if true, container will be array type, else it will be struct type
		 */
		explicit EdbusList(bool a);

		/**
		 * Adds element to the end of the container
		 */
		void append(const EdbusData& val);

		/**
		 * Clears content of container
		 */
		void clear(void);

		/**
		 * Remove element from container, if element is found
		 */
		void remove(const EdbusData& val);

		/**
		 * Remove all elements that are equal to the <em>val</em>
		 */
		void remove_all(const EdbusData& val);

		/**
		 * Compares if two arrays or structs are equal
		 */
		bool operator==(const EdbusList& other) const;

		/**
		 * Compares if two arrays or structs are not equal
		 */
		bool operator!=(const EdbusList& other) const { return !operator==(other); }

		/**
		 * Returns type of values stored in container. Only valid if container is array;
		 * if not, EDBUS_TYPE_INVALID will be returned
		 */
		EdbusDataType value_type(void);

		/**
		 * Return true if value type is container (EdbusDict or EdbusList). Only valid if container is array;
		 * if not, EDBUS_TYPE_INVALID will be returned
		 */
		bool value_type_is_container(void);

		/**
		 * Returns true if object behaves as array
		 */
		bool list_is_array(void) const { return array_mode; }

		/**
		 * Returns true if object behaves as struct
		 */
		bool list_is_struct(void) const { return !array_mode; }

		/**
		 * Returns iterator at the container start. It points to the first element
		 */
		const_iterator begin(void) const;

		/**
		 * Returns iterator at the container end. It <b>does not</b> points to
		 * the last element, but element after the last, and you must not dereferce it
		 */
		const_iterator end(void) const;

		/**
		 * Returns size of container content. This is a constant operation
		 */
		unsigned int size(void) const;

		/**
		 * Explicitly create array. This function should be used to create arrays
		 */
		static EdbusList create_array(void) { return EdbusList(true); }

		/**
		 * Explicitly create struct. This function should be used to create structs 
		 */
		static EdbusList create_struct(void) { return EdbusList(false); }
};

/**
 * A convinient shortcut for EdbusList::append(). You can use it as:
 * \code
 *   EdbusList m = EdbusData::from_struct();
 *   m << EdbusData::from_int32(4) << EdbusData::from_string("sample string") << EdbusData::from_bool(true);
 * \endcode
 */
inline EdbusList& operator<<(EdbusList& lst, const EdbusData& val) {
	lst.append(val);
	return lst;
}

#endif
