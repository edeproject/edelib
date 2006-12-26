#ifndef __UTEST_H__
#define __UTEST_H__

#include <edelib/Diagnose.h>
#include <string.h>

bool equal(char* c1, char* c2)
{
	return (strcmp(c1,c2) == 0);
}

bool notequal(char* c1, char* c2)
{
	return (strcmp(c1,c2) != 0);
}

bool equal(unsigned char* c1, unsigned char* c2)
{
	return (strcmp((const char*)c1,(const char*)c2) == 0);
}

bool notequal(unsigned char* c1, unsigned char* c2)
{
	return (strcmp((const char*)c1,(const char*)c2) != 0);
}


#define TEST_STR_EQUAL(s1, s2)    EASSERT(equal(s1, s2) == true)
#define TEST_STR_NOTEQUAL(s1, s2) EASSERT(equal(s1, s2) == false)
#define TEST_EQUAL(v1, v2)        EASSERT(v1 == v2)
#define TEST_NOTEQUAL(v1, v2)     EASSERT(v1 != v2)

#endif
