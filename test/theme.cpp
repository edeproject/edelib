#include <edelib/Theme.h>
#include <string.h>
#include <stdio.h>
#include "UnitTest.h"

#define CCHARP(str)           ((const char*)str)
#define STR_EQUAL(str1, str2) (strcmp(CCHARP(str1), CCHARP(str2)) == 0)

EDELIB_NS_USE

UT_FUNC(ThemeTestReader, "Test Theme reader")
{
	Theme t;
	UT_VERIFY( t.load("theme.et") == true );
	UT_VERIFY( t.loaded() == true );

	UT_VERIFY( STR_EQUAL(t.author(), "John Foo <john@foo.com>") );
	UT_VERIFY( STR_EQUAL(t.name(), "Demo") );
	UT_VERIFY( STR_EQUAL(t.sample_image(), "img.jpg") );

	char buf[64];
	long lval;

	UT_VERIFY( t.get_item("test style", "item1", buf, sizeof(buf)) );
	UT_VERIFY( STR_EQUAL(buf, "value 1") );

	UT_VERIFY( t.get_item("test style", "item2", buf, sizeof(buf)) );
	UT_VERIFY( STR_EQUAL(buf, "value 2") );

	UT_VERIFY( t.get_item("test style", "item3", lval, 0) );
	UT_VERIFY( lval == 123 );

	/* return false if item wasn't found */
	UT_VERIFY( t.get_item("test style", "item-do-not-exists", lval, -1) == false );
	UT_VERIFY( lval == -1 );

	/* test calling subfunction */
	UT_VERIFY( t.get_item("test style", "item4", lval, 0) );
	UT_VERIFY( lval == 5 );

	t.clear();

	UT_VERIFY( t.loaded() == false );

	UT_VERIFY( t.load("theme.et") == true );

	UT_VERIFY( t.get_item("test_style2", "item1", buf, sizeof(buf)) );
	UT_VERIFY( STR_EQUAL(buf, "you should get this") );
}

UT_FUNC(ThemeTestBad, "Test bad Theme file")
{
	/* tinyscheme should have been modified for this */
	Theme t;
	UT_VERIFY( t.load("theme-bad.et") == false );
	UT_VERIFY( t.loaded() == false );

	UT_VERIFY( t.load("theme-bad2.et") == false );
	UT_VERIFY( t.loaded() == false );

	long lval;
	//UT_VERIFY( t.get_item("test style", "item1", lval, 0) == false );
}
