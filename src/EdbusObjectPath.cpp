#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "EdbusObjectPath.h"

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
	assert(EdbusObjectPath::valid_path(other.path()) == true);
	val = other.val;
}

void EdbusObjectPath::append(const char* str) {
	if(!EdbusObjectPath::valid_element(str))
		return;

	unsigned int sz = val.size();
	if(sz != 1)
		val += "/";
	val += str;
}

void EdbusObjectPath::clear(void) {
	unsigned int len = val.size();
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

#if 0
int main() {
	EdbusObjectPath p;

	printf("%i %i %i %i\n", EdbusObjectPath::valid_path("/dasda/dasd/4324/fsdfsd/bvbv"),
			EdbusObjectPath::valid_path("///dasdas/dasds"),
			EdbusObjectPath::valid_path("dasdas/dasds/"),
			EdbusObjectPath::valid_path("/dasd /dasdasd --/"));

	return 0;
}
#endif
