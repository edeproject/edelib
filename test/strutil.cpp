#include <edelib/StrUtil.h>
#include <string.h>
#include "UnitTest.h"

using namespace edelib;

#define CCHARP(str)           ((const char*)str)

#define STR_EQUAL(str1, str2) (strcmp(CCHARP(str1), CCHARP(str2)) == 0)

UT_FUNC(strtest, "Test strutil")
{
	unsigned char s1[] = "StRiNG1";
	UT_VERIFY( STR_EQUAL("string1", str_tolower(s1)) );

	unsigned char s2[] = "some Sample Of sTrinG";
	UT_VERIFY( STR_EQUAL("some sample of string", str_tolower(s2)) );

	unsigned char s3[] = "sTRinG2";
	UT_VERIFY( STR_EQUAL("STRING2", str_toupper(s3)) );

	unsigned char s4[] = "xxx AbA BbCCbBc xxx !@# . Abb";
	UT_VERIFY( STR_EQUAL("XXX ABA BBCCBBC XXX !@# . ABB", str_toupper(s4)) );

	char s5[] = "  some with spaces";
	UT_VERIFY( STR_EQUAL("some with spaces", str_trimleft(s5)) );

	char s6[] = "				  some with spaces";
	UT_VERIFY( STR_EQUAL("some with spaces", str_trimleft(s6)) );

	char s6a[] = "|				some with spaces";
	UT_VERIFY( STR_EQUAL("|				some with spaces", str_trimleft(s6a)) );

	char s7[] = ".  some with spaces";
	UT_VERIFY( STR_EQUAL(".  some with spaces", str_trimleft(s7)) );

	char s8[] = "some with spaces  ";
	UT_VERIFY( STR_EQUAL("some with spaces", str_trimright(s8)) );

	char s9[] = "some with spaces                |";
	UT_VERIFY( STR_EQUAL("some with spaces                |", str_trimright(s9)) );

	char s10[] = "some without spaces";
	UT_VERIFY( STR_EQUAL("some without spaces", str_trimright(s10)) );

	char s11[] = "\t\t\t\t\tsome\tstring   \t\t";
	UT_VERIFY( STR_EQUAL("some\tstring", str_trim(s11)) );

	char s12[] = "\nsss\n";
	UT_VERIFY( STR_EQUAL("sss", str_trim(s12)) );
}
