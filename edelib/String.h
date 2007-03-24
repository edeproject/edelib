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
		char* data;
		unsigned int alloc;
		unsigned int len;
	public:
		String();
		String(const char* str);
		~String();
		void assign(const char* str);
		void reserve(unsigned int sz);
		void resize(unsigned int newsz, char defval = 0);
		void append(const char* str);
		void clear(void);
		const char* c_str(void) const;
		unsigned int length(void) const   { return len; }
		unsigned int capacity(void) const { return alloc; }

		char& operator[](const int i) { return data[i]; }
		char  operator[](const int i) const { return data[i]; }
		String& operator=(const char* str) { assign(str); return *this; }
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
