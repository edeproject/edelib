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

#ifndef __STRUTIL_H__
#define __STRUTIL_H__

#include "econfig.h"
#include "String.h"

/*
 * Intentionaly outside namespace so I can provide either system
 * or local implementations
 */
#ifndef SKIP_DOCS

unsigned int estrnlen(const char* str, unsigned int maxlen);
char*        estrndup(const char* str, unsigned int maxlen);

#endif // SKIP_DOCS


EDELIB_NAMESPACE {

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

#ifndef SKIP_DOCS
inline bool isws(char c, const char* str) { return (strchr(str, c) != NULL); }
#endif

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
	String::size_type i = 0;
	String::size_type j = 0;

	while(i < sz) {
		while((i < sz) && (isws(str[i], ws)))
			i++;
		if(i == sz)
			return;
		j = i + 1;
		while((j < sz) && (!isws(str[j],ws)))
			j++;

		c.push_back(str.substr(i, j-i));
		i = j + 1;
	}
}

/*
 * This is generic stringtok, but the bad thing is
 * that it char* (or char[]) can't be used as StringType
 * so explicit String constructor must be used;
 * for now commented
 */
#if 0
template <typename Container, typename StringType>
void stringtok(Container& c, const StringType& str, const char* ws = " \t\n") {
	const typename StringType::size_type sz = str.length();
	typename StringType::size_type i = 0;
	typename StringType::size_type j = 0;

	while(i < sz) {
		while((i < sz) && (isws(str[i], ws)))
			i++;
		if(i == sz)
			return;
		j = i + 1;
		while((j < sz) && (!isws(str[j],ws)))
			j++;

		c.push_back(str.substr(i, j-i));
		i = j + 1;
	}
}
#endif

}

#endif
