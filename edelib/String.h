/*
 * $Id$
 *
 * A simple string class.
 * Part of edelib.
 * Copyright (c) 2005-2007 EDE Authors.
 *
 * This program is licenced under terms of the 
 * GNU General Public Licence version 2 or newer.
 * See COPYING for details.
 */

#ifndef __STRING_H__
#define __STRING_H__

#include "econfig.h"
#include <string.h>

EDELIB_NAMESPACE {

/**
 * \class String
 * \brief A (relatively simple) string implementation.
 *
 * This implementation tries to be compatible with
 * std::string implementation, althought it does not implement
 * all gory details from std::string. There are few reasons
 * why this class exists:
 *   - complilation time; things with this class compile 3x faster
 *   than with std::string
 *   - extending; std::string is pain in the ass to extend, and
 *   providing possible copy of std::string implementation would
 *   require to provide a bunch of additional code
 *
 * This class <b>does not</b> provide <em>find_xxx, insert and erase</em> methods and
 * iterators. 
 *
 * Also what is important, this is not COW (Copy-On-Write) implementation since 
 * those have very bad performance in multi-thread world.
 *
 * Some methods, like \e printf() does not exist in std::string. Also, the
 * behaviour of \e capacity() differs from std::string like:
 * \code
 *   String s = "foo";
 *   std::string s1 = "baz";
 *   s.capacity() != s1.capacity()   // implementation specific, according to the Standard
 *
 *   // but...
 *   s.reserve(20);
 *   s1.reserve(20);
 *   s.capacity() == s1.capacity()   // same sizes
 * \endcode
 *
 * So, to sum all things up, in them most code replacing edelib::String
 * with std::string should provide exact behavour (except when printf is used).
 *
 * If you are not familiar with std::string, following things with this
 * class can be done:
 * \code
 *   String s = "sample string";
 *   s += " yes";          // gives "sample string yes"
 *   s.clear();            // clear content
 *   s.assign("foo")       // same as: s = "foo"
 *   s.append("baz")       // append some data, gives "foobaz"
 *   if(s == "foobaz") ... // comparison
 *   if(s > "bla") ...     // check if content of s is grater than "bla"
 *   s += "xxx";           // same as s.append("xxx");
 *   s.find("baz");        // return position of "baz" in "foobaz"
 *   s.find("demo");       // return String::npos, and can be checked like:
 *
 *   if(s.find("demo") == String::npos)
 *     // do something smart
 *   
 *   // not in std::string
 *   s.printf("%s = %i", "num", 4);  // will be "num = 4"
 * \endcode
 *
 * \note
 * Since String increase internal buffer's size when is needed, some things
 * should be considered to minimize reallocations:
 *   - use reserve() if you know the length
 *   - prefer operator+= than operator+
 *
 * Like:
 * \code
 *   String s = "foo";
 *   s.reserve(20)         // keep content, and set internal buffer to 20 characters
 *   s += "baz";           // reallocation not needed
 *   s += "foo";           // not needed; will be needed when we exceed the size
 *
 *   // and...
 *   s = s + "baz";        // is not like below
 *   s += "baz";           // much faster than above
 * \endcode
 *
 * For all these quirks and details about them you should consult some good C++ book :P
 */
class EDELIB_API String 
{
	public: 
#ifndef SKIP_DOCS
		typedef unsigned int size_type;
#endif

	private:
#ifndef SKIP_DOCS
		struct StringData 
		{
			size_type length;
			size_type capacity;
			char      *chars;
		};
#endif
		static StringData null_data;
		StringData* sdata;

		void init(size_type len, size_type cap);
		void dispose(void);

	public:
		/**
		 * This will be returned when find() method fails. If is meant
		 * to be used in form:
		 * \code
		 *   String s;
		 *   if(s.find("this does not exist") == String::npos)
		 *     // do something smart
		 * \endcode
		 */
		static const size_type npos;

		/**
		 * Create empty string object
		 */
		String();

		/**
		 * Create a new string with copy of pointer to characters
		 *
		 * \param str a pointer to c-like string (it should not be NULL)
		 */
		String(const char* str);

		/**
		 * Create a new string with copy of another string
		 *
		 * \param str is object of type String
		 */
		String(const String& str);

		/**
		 * Clears all internal data. All possible external pointers
		 * to internal buffer will be invalidated
		 */
		~String();

		/**
		 * Assign content of c-like string, with given size. This method
		 * will destroy the previous content of the string
		 *
		 * \return itself
		 * \param str a pointer to c-like string (it should not be NULL)
		 * \param len a number of character that will be assigned
		 */
		String& assign(const char* str, size_type len);

		/**
		 * Assign content of c-like string with it's full length.
		 *
		 * \return itself
		 * \param str a pointer to c-like string (it should not be NULL)
		 */
		String& assign(const char* str);

		/**
		 * Assing content of String object
		 *
		 * \return itself
		 * \param str a object of type String
		 */
		String& assign(const String& str);

		/**
		 * Appends content of c-like string, with given length to the end
		 * of current string
		 *
		 * \return itself
		 * \param str a pointer to c-like string (it should not be NULL)
		 * \param len a number of character that will be appended
		 */
		String& append(const char* str, size_type len);

		/**
		 * Appends content of c-like string with it's full length to the
		 * end of current string
		 *
		 * \return itself
		 * \param str a pointer to c-like string (it should not be NULL)
		 */
		String& append(const char* str);

		/**
		 * Appends content of String object to the end of current string
		 *
		 * \return itself
		 * \param str a object of type String
		 */
		String& append(const String& str);


		/**
		 * Appends given character num times at the end of character string
		 *
		 * \return itself
		 * \param num is number of given character
		 * \param ch is character to append
		 */
		String& append(size_type num, const char& ch);

		/**
		 * Set size of internal buffer
		 *
		 * \param len is size we want
		 */
		void reserve(size_type len);

		/**
		 * Exchange the elements of current string with given
		 *
		 * \param from is replacement target
		 */
		void swap(String& from);

		/**
		 * Returns a substring of the current string starting at the
		 * index with num characters long. If num is not specified, returned
		 * will be remain data starting from index
		 *
		 * \return substring
		 * \param index starting position for substring
		 * \param num ending position for substring
		 */
		String substr(size_type index, size_type num = npos) const;

		/**
		 * Returns starting position of str starting at offset. If str is not
		 * found, String::npos will be returned
		 *
		 * \return position of str, or String::npos if not found
		 * \param str is string we are looking for
		 * \param offset position to start looking from
		 */
		size_type find(const char* str, size_type offset) const;

		/**
		 * Returns starting position of given character starting at the given
		 * offset. If character is not found, String::npos will be returned
		 *
		 * \return position of given character, or String::npos if not found
		 * \param ch character we are looking for
		 * \param offset position to start looking from
		 */
		size_type find(char ch, size_type offset) const;

		/**
		 * Returns start of given string. Behaves same as find(str, 0)
		 */
		size_type find(const char* str) const;

		/**
		 * Clear all elements of current string
		 */
		void clear(void);

		/**
		 * Assign data in printf form, like:
		 * \code
		 *   String s;
		 *   s.printf("This is %i number", 3);
		 * \endcode
		 *
		 * All previous content will be deleted.
		 */
		void printf(const char* fmt, ...);

		/** 
		 * Return data formated as c-like string 
		 * 
		 * Can be used as input for C functions, like:
		 * \code
		 *   if(strcmp(s.c_str(), "my smart string") == 0)
		 *     ...
		 * \endcode
		 * */
		const char* c_str(void)  { return sdata->chars; }

		/** Return data formated as c-like string */
		const char* c_str(void) const { return sdata->chars; }

		/** 
		 * Retrun pointer to internal buffer 
		 *
		 * Do \b not use this function as input for C functions.
		 * */
		const char* data(void) const  { return sdata->chars; }

		/** Retrun size of character data */
		size_type   length(void) const { return sdata->length; }

		/** Retrun size of internal buffer */
		size_type  capacity(void) const { return sdata->capacity; }

		/** Checks if string is empty */
		bool       empty(void) const  { return length() == 0; }

		/** Returns character at given index */
		char& operator[](size_type index);

		/** Returns character at given index */
		char  operator[](size_type index) const;

		/** Same as assign(str) */
		String& operator=(const char* str);

		/** Same as assign(String type) */
		String& operator=(const String& str);

		/** Same as append(str) */
		String& operator+=(const char* str);

		/** Same as append(String type) */
		String& operator+=(const String& str);

		/** Same as append(1, ch) */
		String& operator+=(const char& ch);
};

/*
 * How the hell we place this functions on String class page ?
 * When I figure this out, SKIP_DOCS will be removed
 */
#ifndef SKIP_DOCS
String operator+(const String& s1, const String& s2);
String operator+(const char* s1, const String& s2);
String operator+(const String& s1, const char* s2);

inline bool operator==(const String& str1, const char* str2) { return (strcmp(str1.c_str(), str2) == 0); }
inline bool operator!=(const String& str1, const char* str2) { return (strcmp(str1.c_str(), str2) != 0); }
inline bool operator>(const String& str1, const char* str2) { return (strcmp(str1.c_str(), str2) > 0); }
inline bool operator>=(const String& str1, const char* str2) { return (strcmp(str1.c_str(), str2) >= 0); }
inline bool operator<(const String& str1, const char* str2) { return (strcmp(str1.c_str(), str2) < 0); }
inline bool operator<=(const String& str1, const char* str2) { return (strcmp(str1.c_str(), str2) <= 0); }

inline bool operator==(const char* str1, const String& str2) { return (strcmp(str1, str2.c_str()) == 0); }
inline bool operator!=(const char* str1, const String& str2) { return (strcmp(str1, str2.c_str()) != 0); }
inline bool operator>(const char* str1, const String& str2) { return (strcmp(str1, str2.c_str()) > 0); }
inline bool operator>=(const char* str1, const String& str2) { return (strcmp(str1, str2.c_str()) >= 0); }
inline bool operator<(const char* str1, const String& str2) { return (strcmp(str1, str2.c_str()) < 0); }
inline bool operator<=(const char* str1, const String& str2) { return (strcmp(str1, str2.c_str()) <= 0); }

inline bool operator==(const String& str1, const String& str2) 
{ return (str1.length() == str2.length()) && (strcmp(str1.c_str(), str2.c_str()) == 0); }

inline bool operator!=(const String& str1, const String& str2) { return (strcmp(str1.c_str(), str2.c_str()) != 0); }
inline bool operator>(const String& str1, const String& str2) { return (strcmp(str1.c_str(), str2.c_str()) > 0); }
inline bool operator>=(const String& str1, const String& str2) { return (strcmp(str1.c_str(), str2.c_str()) >= 0); }
inline bool operator<(const String& str1, const String& str2) { return (strcmp(str1.c_str(), str2.c_str()) < 0); }
inline bool operator<=(const String& str1, const String& str2) { return (strcmp(str1.c_str(), str2.c_str()) <= 0); }
#endif

}
#endif
