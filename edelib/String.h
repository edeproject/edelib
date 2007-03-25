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

class String 
{
	private:
		typedef unsigned int size_type;

		struct StringData 
		{
			size_type length;
			size_type capacity;
			char      *chars;
		};
		static StringData null_data;
		StringData* sdata;

		void init(size_type len, size_type cap);
		void dispose(void);

	public:
		static const size_type npos;

		String();
		String(const char* str);
		String(const String& str);
		~String();
		void assign(const char* str, size_type len);
		void assign(const char* str);
		void append(const char* str, size_type len);
		void append(const char* str);
		void reserve(size_type len);
		void swap(String& from);
		size_type find(const char* str, size_type offset);
		size_type find(char ch, size_type offset);
		size_type find(const char* str);
		void clear(void);

		const char* c_str(void)  { return sdata->chars; }
		const char* c_str(void) const { return sdata->chars; }
		const char* data(void)  { return sdata->chars; }
		size_type   length(void) const { return sdata->length; }
		size_type  capacity(void) const { return sdata->capacity; }
		bool       empty(void) const  { return length() == 0; }
		char& operator[](size_type index);
		char  operator[](size_type index) const;
		String& operator=(const char* str);
		String& operator+=(const char* str);
};

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

}
#endif
