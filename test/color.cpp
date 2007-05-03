#include <edelib/Color.h>
#include <stdio.h>
#include "UnitTest.h"

using namespace edelib;

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
	UT_VERIFY( color_html_to_fltk("#fff") == color_rgb_to_fltk(455, 285, 3455) );

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
	
}
