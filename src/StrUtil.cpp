/*
 * $Id$
 *
 * Basic functions for C string handling
 * Part of edelib.
 * Copyright (c) 2005-2007 EDE Authors.
 *
 * This program is licenced under terms of the 
 * GNU General Public Licence version 2 or newer.
 * See COPYING for details.
 */

#include <edelib/econfig.h>
#include <edelib/Debug.h>
#include <edelib/StrUtil.h>

#include <ctype.h>
#include <string.h>

EDELIB_NAMESPACE {

char* str_trimleft(char* str)
{
	EASSERT(str != NULL);

	char* ptr;
	for(ptr = str; *ptr && isspace(*ptr); ptr++)
		;
	memmove(str, ptr, strlen(ptr) + 1);
	return str;
}

char* str_trimright(char* str)
{
	EASSERT(str != NULL);

	int len = strlen(str);
	if(!len)
		return str;

	char* p = str + len;
	p--; // go to '\0' - 1
	for(; len >= 0 && isspace(*p); p--, len--)
		;
	// preserve last char
	*(++p) = '\0';
	return str;
}

char* str_trim(char* str)
{
	return str_trimleft(str_trimright(str));
}

unsigned char* str_tolower(unsigned char* str)
{
	EASSERT(str != NULL);

	unsigned char* ptr = str;
	while(*ptr)
	{
		*ptr = tolower(*ptr);
		ptr++;
	}
	return str;
}

unsigned char* str_toupper(unsigned char* str)
{
	EASSERT(str != NULL);

	unsigned char* ptr = str;
	while(*ptr)
	{
		*ptr = toupper(*ptr);
		ptr++;
	}
	return str;
}

bool str_ends(const char* str, const char* test)
{
	EASSERT(str != NULL);
	EASSERT(test != NULL);

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

}
