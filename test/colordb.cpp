#include <edelib/ColorDb.h>
#include <stdio.h>
#include <string.h>
#include "UnitTest.h"

EDELIB_NS_USE

UT_FUNC(ColorDbTest, "Test color database")
{
	unsigned char r,g,b;
	ColorDb c;

	r = g = b = 0;
	UT_VERIFY( c.load() == true );
	UT_VERIFY( c.find("peach puff", r, g, b) == true );	
	UT_VERIFY( r == 255 );
	UT_VERIFY( g == 218 );
	UT_VERIFY( b == 185 );

	r = g = b = 0;
	UT_VERIFY( c.find("lavender blush", r, g, b) == true );
	UT_VERIFY( r == 255 );
	UT_VERIFY( g == 240 );
	UT_VERIFY( b == 245 );

	r = g = b = 0;
	UT_VERIFY( c.find("LavenderBlush", r, g, b) == true );
	UT_VERIFY( r == 255 );
	UT_VERIFY( g == 240 );
	UT_VERIFY( b == 245 );

	UT_VERIFY( c.find("black", r, g, b) == true );
	UT_VERIFY( r == 0 );
	UT_VERIFY( g == 0 );
	UT_VERIFY( b == 0 );

	r = g = b = 0;
	UT_VERIFY( c.find("grey99", r, g, b) == true );
	UT_VERIFY( r == 252 );
	UT_VERIFY( g == 252 );
	UT_VERIFY( b == 252 );

	r = g = b = 0;
	UT_VERIFY( c.find("gray100", r, g, b) == true );
	UT_VERIFY( r == 255 );
	UT_VERIFY( g == 255 );
	UT_VERIFY( b == 255 );

	r = g = b = 0;
	UT_VERIFY( c.find("xxx", r, g, b) == false );
	UT_VERIFY( r == 0 );
	UT_VERIFY( g == 0 );
	UT_VERIFY( b == 0 );
}

UT_FUNC(ColorDbTest2, "Test empty color database")
{
	ColorDb c;
	unsigned char r, g, b;
	r = g = b = 2;

	UT_VERIFY( c.find("foo", r, g, b) == false );
	UT_VERIFY( r == 2 );
	UT_VERIFY( g == 2 );
	UT_VERIFY( b == 2 );

	UT_VERIFY( c.find("", r, g, b) == false );
	UT_VERIFY( r == 2 );
	UT_VERIFY( g == 2 );
	UT_VERIFY( b == 2 );
}
