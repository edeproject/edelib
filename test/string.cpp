#include <edelib/String.h>
#include <stdlib.h>
#include "UnitTest.h"

using namespace edelib;

UT_FUNC(StringBasicTest, "Test basic string functions")
{
	String dstr;
	UT_VERIFY( dstr.length() == 0 );

	// std::string return this
	//UT_VERIFY( dstr.c_str() == "" );

	dstr.reserve(20);
	UT_VERIFY( dstr.capacity() == 20 + 16 );
	UT_VERIFY( dstr.length() == 0 );

	dstr = "Test string";
	UT_VERIFY( dstr.length() == 11 );

	dstr = "Test";
	UT_VERIFY( dstr.length() == 4 );

	dstr.clear();
	UT_VERIFY( dstr.length() == 0 );

	dstr.assign("test");
	UT_VERIFY( dstr.length() == 4 );
	dstr.append(" sample");
	UT_VERIFY( dstr.length() == 11 );
	dstr.append(" sample1");
	dstr.append(" sample2");
	dstr.append("sample3");
	UT_VERIFY( dstr.length() == 34 );
}

UT_FUNC(StringOperators, "Test string operators")
{
	String test = "sample";
	UT_VERIFY( test != "Sample" );
	UT_VERIFY( test == "sample" );
	UT_VERIFY( test <= "sample" );
	UT_VERIFY( test >= "sample" );
	UT_VERIFY( test > "sa" );
	UT_VERIFY( test < "samplebla" );

	UT_VERIFY( "Sample" != test );
	UT_VERIFY( "sample" == test );
	UT_VERIFY( "sample" >= test );
	UT_VERIFY( "sample" <= test );
	UT_VERIFY( "samplebla" > test );
	UT_VERIFY( "sam" < test );

	String test2 = "dumb";
	UT_VERIFY( test != test2 );

	String test3 = "dumb";
	UT_VERIFY( test3 == test2 );
	UT_VERIFY( test3 == test3 );
	UT_VERIFY( test2 == test2 );
	UT_VERIFY( test2 >= test3 );
	UT_VERIFY( test2 <= test3 );
	UT_VERIFY( test >= test2 );
	UT_VERIFY( test > test2 );
	UT_VERIFY( test >= test3 );
	UT_VERIFY( test > test3 );
}
