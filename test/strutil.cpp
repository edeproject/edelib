#include <edelib/StrUtil.h>
#include <string.h>
#include <stdio.h>

#include "Utest.h"

using namespace edelib;


#define UCHARP(str)           ((unsigned char*)str)

int main()
{
	unsigned char s1[] = "StRiNG1";
	TEST_STR_EQUAL(UCHARP("string1"), str_tolower(s1));
	unsigned char s2[] = "some Sample Of sTrinG";
	TEST_STR_EQUAL(UCHARP("some sample of string"), str_tolower(s2));

	unsigned char s3[] = "sTRinG2";
	TEST_STR_EQUAL(UCHARP("STRING2"), str_toupper(s3));
	unsigned char s4[] = "xxx AbA BbCCbBc xxx !@# . Abb";
	TEST_STR_EQUAL(UCHARP("XXX ABA BBCCBBC XXX !@# . ABB"), str_toupper(s4));

	char s5[] = "  some with spaces";
	TEST_STR_EQUAL("some with spaces", str_trimleft(s5));
	char s6[] = "				  some with spaces";
	TEST_STR_EQUAL("some with spaces", str_trimleft(s6));
	char s7[] = ".  some with spaces";
	TEST_STR_EQUAL(".  some with spaces", str_trimleft(s7));

	char s8[] = "some with spaces  ";
	TEST_STR_EQUAL("some with spaces", str_trimright(s8));
	char s9[] = "some with spaces                |";
	TEST_STR_EQUAL("some with spaces                |", str_trimright(s9));
	char s10[] = "some without spaces";
	TEST_STR_EQUAL("some without spaces", str_trimright(s10));

	char s11[] = "\t\t\t\t\tsome\tstring   \t\t";
	TEST_STR_EQUAL("some\tstring", str_trim(s11));

	return 0;
}
