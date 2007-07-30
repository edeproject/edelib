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

#ifndef __COLOR_H__
#define __COLOR_H__

#include "econfig.h"

EDELIB_NS_BEGIN

/**
 * Convert separated rgb color components to FLTK color space.
 *
 * \return color value that can be used for widget color
 * \param r is red component
 * \param g is green component
 * \param b is blue component
 */
EDELIB_API unsigned int color_rgb_to_fltk(unsigned char r, unsigned char g, unsigned char b);

/**
 * Decompose FLTK color to rgb components.
 *
 * \note This function will decompose corectly values grater than 255 or FLTK colors (FL_WHIRE, FL_RED,...). 
 * When FLTK color is given, it will use colormap, the same one used by fltk.
 *
 * \param color is FLTK color 
 * \param r is extracted red component
 * \param g is extracted green component
 * \param b is extracted blue component
 */
EDELIB_API void color_fltk_to_rgb(unsigned int color, unsigned char& r, unsigned char& g, unsigned char& b);

/**
 * Tries to convert html-like color names to FLTK color space. This function
 * does not support named colors, like <em>\#black</em>, but only their
 * hex values, like <em>\#abc</em>. It will fail (return 0) if one of components
 * is missing (eg. given \#a or \#ab).
 *
 * \return color value coresponding to FLTK color space or return 0 (black)
 * \param col is named color, if is NULL, it will return 0 (black)
 */
EDELIB_API unsigned int color_html_to_fltk(const char* col);

/**
 * Convert FLTK color to html-like representation. Given buffer is assumed to
 * be a prior allocated and <b>must</b> be at least 8 character wide. Final result
 * will be in form <em>\#rrggbb</em> and string will be null terminated.
 *
 * \note For this function the same applies as for color_fltk_to_rgb()
 *
 * \param color is FLTK color
 * \param buff is buffer where to place html color
 */
EDELIB_API void color_fltk_to_html(unsigned int color, char* buff);

EDELIB_NS_END
#endif
