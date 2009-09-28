/*
 * $Id$
 *
 * Basic functions for C string handling
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

#ifndef __EDELIB_STRUTIL_H__
#define __EDELIB_STRUTIL_H__

#include "String.h"

EDELIB_NS_BEGIN

/**
 * Removes leading space. Operates directly on buffer.
 */
EDELIB_API char* str_trimleft(char* str);

/**
 * Removes leading space. Operates directly on buffer.
 */
EDELIB_API char* str_trimright(char* str);

/**
 * Same as str_trimleft(str_trimright(str)).
 */
EDELIB_API char* str_trim(char* str);

/**
 * Makes all characters lower. Operates directly on buffer.
 */
EDELIB_API unsigned char* str_tolower(unsigned char* str);

/**
 * Makes all characters upper. Operates directly on buffer.
 */
EDELIB_API unsigned char* str_toupper(unsigned char* str);

/**
 * Check if string ends with given test string.
 *
 * \return true if ends or false if not
 * \param str is where to search
 * \param test is what to search
 * */
EDELIB_API bool str_ends(const char* str, const char* test);

/**
 * A generic String tokenizer (or spliter), similar to stringtok, but works on
 * given container (container that have push_back()). 
 *
 * If delimiter is not found * in container will be put whole string parameter 
 * (or container will be size 1). If given string is empty, in container will
 * not be put anything.
 *
 * This tokenizer is found in libstdc++ documentation. Author is unknown.
 *
 * \param c is container where will be tokenized data placed
 * \param str is string which should be tokenized
 * \param ws is delimiter by which we do splitting
 */
template <typename Container>
void stringtok(Container& c, const String& str, const char* ws = " \t\n") {
	const String::size_type sz = str.length();
	String::size_type i = 0, j = 0;

	while(i < sz) {
		while((i < sz) && (strchr(ws, str[i]) != NULL))
			i++;
		if(i == sz)
			return;
		j = i + 1;
		while((j < sz) && (strchr(ws, str[j]) == NULL))
			j++;

		c.push_back(str.substr(i, j-i));
		i = j + 1;
	}
}

EDELIB_NS_END
#endif
