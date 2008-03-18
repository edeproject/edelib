/*
 * $Id$
 *
 * D-Bus stuff
 * Part of edelib.
 * Copyright (c) 2008 EDE Authors.
 *
 * This program is licenced under terms of the 
 * GNU General Public Licence version 2 or newer.
 * See COPYING for details.
 */

#include <stdlib.h>
#include <string.h>

#include <edelib/EdbusObjectPath.h>
#include <edelib/Debug.h>

EDELIB_NS_BEGIN

EdbusObjectPath::EdbusObjectPath() {
	val = "/";
}

EdbusObjectPath::~EdbusObjectPath() {
}

EdbusObjectPath::EdbusObjectPath(const char* str) {
	if(!EdbusObjectPath::valid_path(str))
		return;
	val = str;
}

EdbusObjectPath::EdbusObjectPath(const EdbusObjectPath& other) {
	E_ASSERT(EdbusObjectPath::valid_path(other.path()) == true);
	val = other.val;
}

void EdbusObjectPath::append(const char* str) {
	if(!EdbusObjectPath::valid_element(str))
		return;

	unsigned int sz = val.length();
	if(sz != 1)
		val += "/";
	val += str;
}

void EdbusObjectPath::clear(void) {
	unsigned int len = val.length();
	/* do nothing if we are 'empty' one */
	if(len == 1 && val[0] == '/')
		return;

	val.clear();
	val = "/";
}

/* static */
bool EdbusObjectPath::valid_element(const char* str) {
	const char* c = str;
	while(*c) {
		if(!((*c >= 'A' && *c <= 'Z') || (*c >= 'a' && *c <='z') || (*c >= '0' && *c <= '9') || *c == '_'))
			return false;
		c++;
	}
	return true;
}

/* static */
bool EdbusObjectPath::valid_path(const char* str) {
	unsigned int len = strlen(str);

	if(!len)
		return false;

	if(str[0] != '/' || str[len-1] == '/')
		return false;

	/* 
	 * make sure we didn't get '//' or more because tokenizator will 
	 * remove all slash characters
	 */
	if(strstr(str, "//"))
		return false;

	/* now tokenize everything */
	char* copy = strdup(str);
	char* saveptr; 
	char* tok = strtok_r(copy, "/", &saveptr);
	bool ret = true;

	while(tok != NULL) {
		if(!valid_element(tok)) {
			ret = false;
			break;
		}

		tok = strtok_r(NULL, "/", &saveptr);
	}

	free(copy);
	return ret;
}

EDELIB_NS_END
