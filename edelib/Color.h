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

EDELIB_NAMESPACE {

/**
 * Convert separated rgb color components to FLTK color space.
 *
 * \return color value that can be used for widget color
 * \param r is red component
 * \param g is green component
 * \param b is blue component
 */
EDELIB_API unsigned int color_rgb_to_fltk(unsigned int r, unsigned int g, unsigned int b);

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

}
#endif
