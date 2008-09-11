#include <edelib/Color.h>
#include <stdio.h>
#include <string.h>
#include "UnitTest.h"

#define CCHARP(str)           ((const char*)str)
#define STR_EQUAL(str1, str2) (strcmp(CCHARP(str1), CCHARP(str2)) == 0)

EDELIB_NS_USE

UT_FUNC(ColorTest, "Test color conversions")
{
	UT_VERIFY( color_rgb_to_fltk(0, 0, 0) == 0 );
	UT_VERIFY( color_rgb_to_fltk(255, 255, 255) == 0xffffff00 );
	UT_VERIFY( color_html_to_fltk("#97aa8e") == color_rgb_to_fltk(151, 170, 142) );
	UT_VERIFY( color_html_to_fltk("#36492c") == color_rgb_to_fltk(54, 73, 44) );
	UT_VERIFY( color_html_to_fltk("#000") == color_rgb_to_fltk(0, 0, 0) );
	UT_VERIFY( color_html_to_fltk("#fff") == color_rgb_to_fltk(255, 255, 255) );
	UT_VERIFY( color_html_to_fltk("#ffffff") == color_rgb_to_fltk(255, 255, 255) );
	UT_VERIFY( color_html_to_fltk("#fffffffff") == color_rgb_to_fltk(255, 255, 255) );

	// incorrect rgb values
	//UT_VERIFY( color_html_to_fltk("#fff") == color_rgb_to_fltk(455, 285, 3455) );

	// incorrect triplets
	UT_VERIFY( color_html_to_fltk("#ffff") == color_rgb_to_fltk(0, 0, 0) );
	UT_VERIFY( color_html_to_fltk("#fffff") == color_rgb_to_fltk(0, 0, 0) );
	UT_VERIFY( color_html_to_fltk("#fffffab") == color_rgb_to_fltk(0, 0, 0) );
	UT_VERIFY( color_html_to_fltk("#b") == color_rgb_to_fltk(0, 0, 0) );
	UT_VERIFY( color_html_to_fltk("#") == color_rgb_to_fltk(0, 0, 0) );
	UT_VERIFY( color_html_to_fltk("fff") == color_rgb_to_fltk(0, 0, 0) );
	UT_VERIFY( color_html_to_fltk("##fff") == color_rgb_to_fltk(0, 0, 0) );
	UT_VERIFY( color_html_to_fltk("###fff") == color_rgb_to_fltk(0, 0, 0) );

	// yes, we can accept NULL
	UT_VERIFY( color_html_to_fltk(NULL) == color_rgb_to_fltk(0, 0, 0) );

	// reverse
	
	unsigned char r,g,b;	

	// black, but can only work for this color; for others we need colormap lookup
	color_fltk_to_rgb(0x38, r, g, b);
	UT_VERIFY( r == 0 );
	UT_VERIFY( g == 0 );
	UT_VERIFY( b == 0 );

	// white
	color_fltk_to_rgb(0xffffff00, r, g, b);
	UT_VERIFY( r == 255 );
	UT_VERIFY( g == 255 );
	UT_VERIFY( b == 255 );
	

	char buff[8];
	color_fltk_to_html( color_rgb_to_fltk(151, 170, 142), buff );
	UT_VERIFY( STR_EQUAL(buff, "#97aa8e") );

	color_fltk_to_html( color_rgb_to_fltk(54, 73, 44), buff );
	UT_VERIFY( STR_EQUAL(buff, "#36492c") );

	color_fltk_to_html( color_rgb_to_fltk(255, 255, 255), buff );
	UT_VERIFY( STR_EQUAL(buff, "#ffffff") );

	color_fltk_to_html( color_rgb_to_fltk(0, 0, 0), buff );
	UT_VERIFY( STR_EQUAL(buff, "#000000") );
}


UT_FUNC(ColorTest2, "Test color conversions2")
{
	unsigned char r,g,b;
	// FL_WHITE
	color_fltk_to_rgb(255, r, g, b);

	UT_VERIFY( r == 255 );
	UT_VERIFY( g == 255 );
	UT_VERIFY( b == 255 );

	// FL_BLACK (colormap)
	color_fltk_to_rgb(56, r, g, b);

	UT_VERIFY( r == 0 );
	UT_VERIFY( g == 0 );
	UT_VERIFY( b == 0 );

	// FL_RED (colormap)
	color_fltk_to_rgb(88, r, g, b);

	UT_VERIFY( r == 255 );
	UT_VERIFY( g == 0 );
	UT_VERIFY( b == 0 );

	// FL_GREEN (colormap)
	color_fltk_to_rgb(63, r, g, b);

	UT_VERIFY( r == 0 );
	UT_VERIFY( g == 255 );
	UT_VERIFY( b == 0 );

	// FL_BLUE (colormap)
	color_fltk_to_rgb(216, r, g, b);

	UT_VERIFY( r == 0 );
	UT_VERIFY( g == 0 );
	UT_VERIFY( b == 255 );

	// FL_YELLOW (colormap)
	color_fltk_to_rgb(95, r, g, b);

	UT_VERIFY( r == 255 );
	UT_VERIFY( g == 255 );
	UT_VERIFY( b == 0 );
	
	// FL_MAGENTA (colormap)
	color_fltk_to_rgb(248, r, g, b);

	UT_VERIFY( r == 255 );
	UT_VERIFY( g == 0 );
	UT_VERIFY( b == 255 );
}
