#include <edelib/econfig.h>
#include <edelib/Diagnose.h>
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
		*ptr++ = tolower(*ptr);
	return str;
}

unsigned char* str_toupper(unsigned char* str)
{
	EASSERT(str != NULL);

	unsigned char* ptr = str;
	while(*ptr)
		*ptr++ = toupper(*ptr);
	return str;
}

}
