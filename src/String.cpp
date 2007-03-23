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

#include <edelib/econfig.h>
#include <edelib/String.h>
#include <string.h>

EDELIB_NAMESPACE {

String::String() : data(0), alloc(0), len(0)
{ 
}

String::String(const char* str) : data(0), alloc(0), len(0)
{
	assign(str);
}

String::~String()
{
	if(data)
	{
		//printf("clearing\n");
		delete[] data;
	}
}

void String::reserve(unsigned int size)
{
	if(alloc < size)
	{
		alloc += size + 16;
		char* newstr = new char[alloc];
		if(data)
		{
			memcpy(newstr, data, len);
			delete[] data;
		}
		data = newstr;
	}
}

void String::assign(const char* str)
{
	unsigned int old = len;
	len = strlen(str);
	reserve(len);
	memcpy(data, str, len);

	// pseudo shrinker
	if(len < old)
		data[len] = '\0';
}

void String::resize(unsigned int newsz, char defval)
{
	// pseudo shrinker
	if(newsz < len)
	{
		len = newsz;
		data[len] = '\0';
		return;
	}

	// we are bigger, so
	// len have to be exact size as newsz
	reserve(newsz);

	char* ptr = data + len;
	for(unsigned int i = 0; i < newsz; i++, ptr++)
		*ptr = defval;

	len = newsz;
	data[len] = '\0';
}

void String::append(const char* str)
{
	unsigned int lstr = strlen(str);
	reserve(len + lstr);
	char* last = data + len;
	
	for(unsigned int i = 0; i < lstr; i++, last++, str++)
		*last = *str;
	len += lstr;
	data[len] = '\0';
}

void String::clear(void)
{
	len = 0;
	data[len]='\0';
}

const char* String::c_str(void) const
{
	if(!data)
		return "";
	return data;
}

}
