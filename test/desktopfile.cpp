#include <edelib/DesktopFile.h>
#include <string.h>
#include <stdlib.h>
#include "UnitTest.h"

#define CCHARP(str)           ((const char*)str)
#define STR_EQUAL(str1, str2) (strcmp(CCHARP(str1), CCHARP(str2)) == 0)

using namespace edelib;

UT_FUNC(DesktopFileTest, "Test DesktopFile class")
{
	DesktopFile desk;
	desk.load("test.desktop");
	if(!desk) {
		UT_FAIL("No test.desktop, but expected to be");
		return;
	}

	char buff[256];
	// URL
	UT_VERIFY( desk.url(buff, 256) == true );
	UT_VERIFY( STR_EQUAL(buff, "file:$HOME") );
	UT_VERIFY( desk.type() == DESK_FILE_TYPE_LINK );

	// Icon
	UT_VERIFY( desk.icon(buff, 256) == true );
	UT_VERIFY( STR_EQUAL(buff, "folder_home") );

	// Name
	UT_VERIFY( desk.name(buff, 256) == true );
	UT_VERIFY( STR_EQUAL(buff, "Home Folder") );

	// Comment
	UT_VERIFY( desk.comment(buff, 256) == true );
	UT_VERIFY( STR_EQUAL(buff, "This folder contains your personal files") );
}

/*
UT_FUNC(DesktopFileTestXXX, "Test DesktopFile classXXX")
{
	DesktopFile desk;
	desk.set_name("IMEEE");
	desk.set_generic_name("IMEEE ali ovaj put genericko");

	desk.save("xxx.desktop");
}
*/
