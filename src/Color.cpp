/*
 * $Id$
 *
 * Color conversion functions
 * Part of edelib.
 * Copyright (c) 2005-2007 EDE Authors.
 *
 * This program is licenced under terms of the 
 * GNU General Public Licence version 2 or newer.
 * See COPYING for details.
 */

#include <edelib/Color.h>
#include <string.h> // strlen
#include <stdlib.h> // strtol

EDELIB_NAMESPACE {

unsigned int hex2int(char hex) 
{
	int val = -1;

	if(hex >= 'A' && hex <= 'F')
		val = hex - 'A' + 10;
	if(hex >= 'a' && hex <= 'f')
		val = hex - 'a' + 10;
	// then is a number
	if(hex >= '0' && hex <= '9')
		val = hex - '0';

	return val;
}

#define FIX_COL(c) if(c > 255) c = 255

unsigned int color_rgb_to_fltk(unsigned int r, unsigned int g, unsigned int b) 
{
	FIX_COL(r);
	FIX_COL(g);
	FIX_COL(b);

	return (unsigned int) ((r << 24) + (g << 16) + (b << 8));
}

unsigned int color_html_to_fltk(const char* col) 
{
	if(!col || col[0] != '#')
		return 0;    // return black if we fail

	int r, g, b;
	int len = strlen(col);

	// skip '#'
	len -= 1;

	/*
	 * To do conversion correctly we must have rgb triplet
	 * and it can be in form:
	 *  rgb
	 *  rrggbb
	 *  rrrgggbbb
	 */
	if((len % 3) == 0 && (len > 3)) {
		int rgb = strtol(col + 1, NULL, 16);

		r = rgb >> 16;
		g = (rgb >> 8) & 255;
		b = rgb & 255;

	} else if(len == 3) {
		// above part incorectly compute '#rgb'
		r = hex2int(col[1]) * 0x11;
		g = hex2int(col[2]) * 0x11;
		b = hex2int(col[3]) * 0x11;

	} else 
		return 0;

	return color_rgb_to_fltk(r, g, b);
}

}
