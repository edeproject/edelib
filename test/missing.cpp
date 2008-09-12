#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <edelib/Missing.h>
#include <edelib/Directory.h>
#include <edelib/File.h>

#include "UnitTest.h"

#define CCHARP(str)           ((const char*)str)
#define STR_EQUAL(str1, str2) (strcmp(CCHARP(str1), CCHARP(str2)) == 0)

EDELIB_NS_USE

UT_FUNC(MissingTest, "Test various missing funcs")
{
	// setenv/getenv family
	UT_VERIFY( edelib_setenv("SOME_FOO_ENV", "3", 1) == 0 );
	UT_VERIFY( edelib_setenv("SOME_FOO_ENV_TRY_2", "foo foo", 1) == 0 );

	char* ptr;
	ptr = getenv("SOME_FOO_ENV");
	UT_VERIFY( ptr != NULL );
	UT_VERIFY( STR_EQUAL(ptr, "3") );

	ptr = getenv("SOME_FOO_ENV_TRY_2");
	UT_VERIFY( ptr != NULL );
	UT_VERIFY( STR_EQUAL(ptr, "foo foo") );

	// edelib_strnlen
	const char* txt = "Some sample text without meaning";
	unsigned int txt_len = 32;
	UT_VERIFY( edelib_strnlen(txt, 1024) == txt_len );
	UT_VERIFY( edelib_strnlen(txt, 33) == txt_len );
	UT_VERIFY( edelib_strnlen(txt, 10) == 10 );
	UT_VERIFY( edelib_strnlen(txt, 1) == 1 );

	// edelib_strndup
	char* val = edelib_strndup(txt, 1024);
	UT_VERIFY( STR_EQUAL(val, txt) );
	free(val);

	val = edelib_strndup(txt, 10);
	UT_VERIFY( STR_EQUAL(val, "Some sampl") );
	free(val);

	val = edelib_strndup(txt, 1);
	UT_VERIFY( STR_EQUAL(val, "S") );
	free(val);

	val = edelib_strndup(txt, txt_len);
	UT_VERIFY( STR_EQUAL(val, txt) );
	free(val);

	// edelib_strlcpy
	char buff[1024];
	UT_VERIFY( edelib_strlcpy(buff, "foo", 5) == 3 );
	UT_VERIFY( STR_EQUAL(buff, "foo") );

	UT_VERIFY( edelib_strlcpy(buff, txt, 1024) == txt_len );
	UT_VERIFY( STR_EQUAL(buff, txt) );

	char small_buff[3];
	UT_VERIFY( edelib_strlcpy(small_buff, txt, 3) == txt_len );
	UT_VERIFY( STR_EQUAL(small_buff, "So") );

	UT_VERIFY( edelib_strlcpy(small_buff, txt, 1) == txt_len );
	UT_VERIFY( small_buff[0] == '\0' );

	// edelib_strlcat
	edelib_strlcpy(buff, "foo", 5);
	UT_VERIFY( edelib_strlcat(buff, "sample demo", 1024) == 14 ); // strlen("foo") + strlen("sample demo")
	UT_VERIFY( STR_EQUAL(buff, "foosample demo") );

	edelib_strlcpy(small_buff, "x", 3);
	UT_VERIFY( edelib_strlcat(small_buff, "foo", 3) == 4 );
	UT_VERIFY( STR_EQUAL(small_buff, "xf") );

	edelib_strlcat(small_buff, "dasddsdsd", 3);
	UT_VERIFY( STR_EQUAL(small_buff, "xf") );
}

#define COMPARE_FILE_AND_CLEAN(f1, f2)      \
	UT_VERIFY( STR_EQUAL(f1->d_name, f2) ); \
	free(f1);                               \
	file_remove("scandir-test/" f2)

UT_FUNC(ScandirTest, "Test edelib_scandir")
{
	char buff[256];

	dir_create("scandir-test");

	FILE *f;
	for(int i = 0; i < 15; i++) {
		sprintf(buff, "scandir-test/file%i.txt", i);
		f = fopen(buff, "w");
		fclose(f);
	}

	struct dirent **namelist;
	int n = edelib_scandir("scandir-test", &namelist, 0, edelib_alphasort);

	// 15 entries + '.' + '..'
	UT_VERIFY( n == 17 );
	UT_VERIFY( STR_EQUAL(namelist[0]->d_name, ".") );
	UT_VERIFY( STR_EQUAL(namelist[1]->d_name, "..") );

	free(namelist[0]);
	free(namelist[1]);

	// we are not doing version sort so names with
	// numbers are sorted differently
	
	COMPARE_FILE_AND_CLEAN(namelist[2], "file0.txt");
	COMPARE_FILE_AND_CLEAN(namelist[3], "file1.txt");
	COMPARE_FILE_AND_CLEAN(namelist[4], "file10.txt");
	COMPARE_FILE_AND_CLEAN(namelist[5], "file11.txt");
	COMPARE_FILE_AND_CLEAN(namelist[6], "file12.txt");
	COMPARE_FILE_AND_CLEAN(namelist[7], "file13.txt");
	COMPARE_FILE_AND_CLEAN(namelist[8], "file14.txt");
	COMPARE_FILE_AND_CLEAN(namelist[9], "file2.txt");
	COMPARE_FILE_AND_CLEAN(namelist[10], "file3.txt");
	COMPARE_FILE_AND_CLEAN(namelist[11], "file4.txt");
	COMPARE_FILE_AND_CLEAN(namelist[12], "file5.txt");
	COMPARE_FILE_AND_CLEAN(namelist[13], "file6.txt");
	COMPARE_FILE_AND_CLEAN(namelist[14], "file7.txt");
	COMPARE_FILE_AND_CLEAN(namelist[15], "file8.txt");
	COMPARE_FILE_AND_CLEAN(namelist[16], "file9.txt");

	free(namelist);
	dir_remove("scandir-test");
}
