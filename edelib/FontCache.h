/*
 * $Id: File.h 2967 2009-12-02 14:31:34Z karijes $
 *
 * Font database and cache facility
 * Copyright (c) 2005-2011 edelib authors
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __EDELIB_FONTCACHE_H__
#define __EDELIB_FONTCACHE_H__

#include "edelib-global.h"
#include <FL/Enumerations.H>

EDELIB_NS_BEGIN

/**
 * \defgroup fontcache Font Cache functions
 * Functions for for accessing available fonts, caching them and loading with readable names.
 *
 * FLTK does not have support for any kind of system font manipulation except to load all available fonts and
 * to access them by registered index. This strategy is inefficient and quite limited as there is no way to
 * manualy specify font name (like is done with Pango) and size, e.g. <em>sans 10</em>.
 */

/**
 * \ingroup fontcache
 * Initialize font cache database on given path and return if done succesfully. This function will call
 * <em>Fl::set_fonts("-*")</em>, which will in turn open display and register all available fonts to FLTK.
 *
 * This function is not meant to be used directly from application that wants to access font faces. Calling it,
 * it will register unnecessary fonts in FLTK and will increase application memory usage. Here it is so external
 * tools can use it.
 */
bool font_cache_init(const char *dir, const char *db = "edelib-font-cache", const char *prefix = "ede");

/**
 * \ingroup fontcache
 * Call font_cache_init() with <em>user_cache_dir()</em> path.
 */
bool font_cache_init(void);

/**
 * \ingroup fontcache
 * Try to find given face and size in given database path. If found, register it as FLTK font and set font id
 * and size.
 *
 * Font faces should have readable form, like:
 * \verbatim
 *   DejaVu Sans 10
 *   DejaVu Sans Bold Italic 15
 * \endverbatim
 */
bool font_cache_get_by_name(const char *dir, const char *face, Fl_Font &f, Fl_Fontsize &s, const char *db = "edelib-font-cache", const char *prefix = "ede");

/**
 * \ingroup fontcache
 * Call font_cache_get_by_name() with <em>user_cache_dir()</em> path.
 */
bool font_cache_get_by_name(const char *face, Fl_Font &f, Fl_Fontsize &s);

EDELIB_NS_END

#endif
