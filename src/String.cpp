/*
 * $Id$
 *
 * A simple string class
 * Copyright (c) 2005-2007 edelib authors
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

#include <stdarg.h>
#include <stdio.h> // vsnprintf

#include <edelib/String.h>
#include <edelib/Debug.h>
#include <edelib/StrUtil.h>

#define PRINTF_BUF 265
#define STERM '\0'

EDELIB_NS_BEGIN

// copy character num times into dest
static void chcpy(char* dest, const char& ch, String::size_type num) {
	for(char* ptr = dest; num > 0; ptr++, num--)
		*ptr = ch;
}

String::StringData String::null_data = {0, 0, (char*)""};
const String::size_type  String::npos = ~(String::size_type)0;

String::String() : sdata(&null_data)
{ }

String::String(const char* str) : sdata(&null_data) 
{ 
	assign(str); 
}

String::String(const String& str) : sdata(&null_data)
{ 
	if(str.length())
		assign(str.c_str()); 
}

String::~String()
{
	dispose();
}

void String::init(size_type len, size_type cap) {
	E_ASSERT(len <= cap);

	sdata = new StringData;
	sdata->chars = new char[cap + 1];
	sdata->chars[0] = STERM;
	sdata->length = len;
	sdata->capacity = cap;
}

void String::dispose(void) {
	if(sdata != &null_data) {
		delete [] sdata->chars;
		delete sdata;
		sdata = &null_data;
	}
}

void String::reserve(size_type cap) {
	if(cap > capacity()) {
		String tmp;
		tmp.init(length(), cap);
		memcpy(tmp.sdata->chars, data(), length());
		swap(tmp);
	}
}

void String::swap(String& from) {
	StringData* d = sdata;
	sdata = from.sdata;
	from.sdata = d;
}

String& String::assign(const char* str, size_type len) {
	E_ASSERT(str != NULL);
	/*
	 * I'm not implementing max_size().
	 * Here will be checked against only half, so memcpy does not goes crazy
	 */
	E_ASSERT(len < (String::npos / 2) && "Allocation exceeded max allowed size");

	/*
	 * Handle cases:
	 *    foo.reserve(20);
	 *    foo.assign("foo");
	 * since std::string will have:
	 *    foo.length() == 3;
	 *    foo.capacity() == 20;
	 */
	size_type cap = capacity();
	dispose();

	if(len < cap)
		init(len, cap);
	else
		init(len, len);
	memcpy(sdata->chars, str, len);
	sdata->chars[len] = STERM;

	return *this;
}

String& String::assign(const char* str) {
	assign(str, strlen(str));
	return *this;
}

String& String::assign(const String& str) {
	assign(str.c_str(), str.length());
	return *this;
}

String& String::append(const char* str, size_type len) {
	if(len == 0)
		return *this;

	if(len + length() <= capacity()) {
		memcpy(sdata->chars + length(), str, len);
		sdata->length += len;
		sdata->chars[sdata->length] = STERM;
	} else {
		reserve((capacity() + len) * 2);
		memcpy(sdata->chars + length(), str, len);
		sdata->length += len;
		sdata->chars[sdata->length] = STERM;
	}

	return *this;
}

String& String::append(size_type num, const char& ch) {
	if(num + length() <= capacity()) {
		chcpy(sdata->chars + length(), ch, num);
		sdata->length += num;
		sdata->chars[sdata->length] = STERM;
	} else {
		char* buff = new char[num];
		chcpy(buff, ch, num);
		append(buff, num);
		delete[] buff;
	}

	return* this;
}

String& String::append(const char* str) {
	append(str, strlen(str));
	return *this;
}

String& String::append(const String& str) {
	append(str.c_str(), str.length());
	return *this;
}

void String::clear(void) {
	dispose();
}

void String::printf(const char* fmt, ...) {
	E_ASSERT(fmt != NULL);

	char buf[PRINTF_BUF];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buf, PRINTF_BUF, fmt, ap);
	va_end(ap);

	assign((char*)buf);
}

void String::trim_left(void) {
	if(length()) {
		str_trimleft(sdata->chars);
		/* update length */
		sdata->length = strlen(sdata->chars);
	}
}

void String::trim_right(void) {
	if(length()) {
		str_trimright(sdata->chars);
		/* update length */
		sdata->length = strlen(sdata->chars);
	}
}

void String::trim(void) {
	if(length()) {
		str_trim(sdata->chars);
		/* update length */
		sdata->length = strlen(sdata->chars);
	}
}

String& String::operator=(const char* str) {
	assign(str);
	return *this;
}

String& String::operator=(const String& str) {
	if(&str != this)
		assign(str);

	return *this;
}

String& String::operator+=(const char* str) {
	return append(str);
}

String& String::operator+=(const String& str) {
	return append(str);
}

String& String::operator+=(const char& ch) {
	return append(1, ch);
}

char& String::operator[](size_type index) {
	E_ASSERT(index < length());

	return sdata->chars[index];
}

char String::operator[](size_type index) const {
	E_ASSERT(index < length());

	return sdata->chars[index];
}

String String::substr(size_type index, size_type num) const {
	String tmp;
	if(num == npos)
		tmp.assign(data() + index, length() - index);
	else
		tmp.assign(data() + index, num);
	return tmp;
}

String::size_type String::find(const char* str, size_type offset) const {
	if(offset >= length())
		return npos;

	char* p = (char*)strstr(data() + offset, str);
	if(!p)
		return npos;
	else
		return (p - data());
}

String::size_type String::find(const char* str) const {
	return find(str, 0);
}

String::size_type String::find(char ch, size_type offset) const {
	if(offset >= length())
		return npos;

	size_type i = 0;
	for(const char* p = data() + offset; *p != STERM && i < length(); p++, i++) {
		if(*p == ch)
			return (i + offset);
	}
	return npos;
}

String& String::replace(char c1, char c2) {
	if(!length())
		return *this;

	/* 
	 * Never allow to replace terminating character or to add one.
	 * TODO: the same must be for append() members
	 */
	E_ASSERT(c1 != STERM && c2 != STERM && "Replacing (or using as replacement) 0 character is not allowed");

	if(c1 == c2)
		return *this;

	size_type i = 0;
	for(char* p = (char*)data(); *p != STERM && i < length(); p++, i++) {
		if(*p == c1)
			*p = c2;
	}

	return *this;
}

String operator+(const String& s1, const String& s2) {
	String tmp;

	String::size_type len = s1.length();
	len += s1.length();
	/*
	 * Do not allocate anything if sum of lenghts of 
	 * s1 and s2 are 0.
	 */
	if(len) {
		tmp.reserve(len);
		tmp += s1;
		tmp += s2;
	}
	return tmp;
}

String operator+(const char* s1, const String& s2) {
	String tmp;

	String::size_type len = strlen(s1);
	len += s2.length();

	// see above for allocation
	if(len) {
		tmp.reserve(len);
		tmp += s1;
		tmp += s2;
	}
	return tmp;
}

String operator+(const String& s1, const char* s2) {
	String tmp;

	String::size_type len = strlen(s2);
	len += s1.length();

	// see above for allocation
	if(len) {
		tmp.reserve(len);
		tmp += s1;
		tmp += s2;
	}
	return tmp;
}

EDELIB_NS_END
