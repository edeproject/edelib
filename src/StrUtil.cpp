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

#include <ctype.h>
#include <string.h>
#include <edelib/Debug.h>
#include <edelib/StrUtil.h>

EDELIB_NS_BEGIN

char* str_trimleft(char* str) {
	E_ASSERT(str != NULL);

	char* ptr;
	for(ptr = str; *ptr && isspace(*ptr); ptr++)
		;
	memmove(str, ptr, strlen(ptr) + 1);
	return str;
}

char* str_trimright(char* str) {
	E_ASSERT(str != NULL);

	int len = strlen(str);
	if(!len)
		return str;

#if 0
	char* p = str + len;
	p--; // go to '\0' - 1
	for(; len >= 0 && isspace(*p); p--, len--)
		;
	// preserve last char
	*(++p) = '\0';
#endif

	/* 
	 * valgrind somehow reports for above code invalid writing 1 character that
	 * corrupts other part of the program (goes beyond 1 character)
	 */
	int i;
	for(i = len - 1; i >= 0 && isspace(str[i]); i--)
		;

	str[i+1] = 0;
	return str;
}

char* str_trim(char* str) {
	return str_trimleft(str_trimright(str));
}

unsigned char* str_tolower(unsigned char* str) {
	E_ASSERT(str != NULL);

	unsigned char* ptr = str;
	while(*ptr)
	{
		*ptr = tolower(*ptr);
		ptr++;
	}
	return str;
}

unsigned char* str_toupper(unsigned char* str) {
	E_ASSERT(str != NULL);

	unsigned char* ptr = str;
	while(*ptr)
	{
		*ptr = toupper(*ptr);
		ptr++;
	}
	return str;
}

bool str_ends(const char* str, const char* test) {
	E_ASSERT(str != NULL);
	E_ASSERT(test != NULL);

	int len = strlen(str);
	int tlen = strlen(test);
	if((tlen > len) || !len || !tlen)
		return false;

	// cases when 'test' contains only one character
	if(tlen == 1)
		return (str[len-1] == test[0]);

	const char* p = str + len - 1;
	const char* tp = test + tlen - 1;
	for(; tlen; p--, tp--, tlen--)
	{
		if(*p != *tp)
			return false;
	}
	return true;
}

EDELIB_NS_END
