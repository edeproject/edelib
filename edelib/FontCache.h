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

struct FontCache_P;

/**
 * \class FontCache
 * \brief Allow readable font names and cache their access
 */
class FontCache {
private:
	FontCache_P *priv;
	E_DISABLE_CLASS_COPY(FontCache)
public:
	/** Constructor. */
	FontCache() : priv(NULL) {}

	/** Destructor; closes any remaining database handlers. */
	~FontCache() { clear(); }

	/** 
	 * Try to load database in given folder with given name. If fails somehow (database does not exists or
	 * it is not valid), it will return false.
	 */
	bool load(const char *dir, const char *db = "edelib-font-cache", const char *prefix = "ede");

	/** Call load() with <em>user_cache_dir()</em> path. */
	bool load(void);

	/** Unload database (if loaded) and explicitly clear all internal data. */
	void clear(void);

	/**
	 * Try to find given face and size in given database path. If found, register it as FLTK font and set font id
	 * and size.
 	 */
	bool find(const char *n, Fl_Font &font, int &size);

	/**
	 * Initialize font cache database on given path and return number of stored fonts. It will call
	 * <em>Fl::set_fonts("-*")</em>, which will in turn open display and register all available fonts to FLTK.
	 *
	 * This function is not meant to be used directly from application that wants to access font faces. Calling it,
	 * it will register unnecessary fonts in FLTK and will increase application memory usage. Here it is so external
	 * tools can use it.
	 */
	static int init_db(const char *dir, const char *db = "edelib-font-cache", const char *prefix = "ede");

	/** Call init_db() with <em>user_cache_dir()</em> path. */
	static int init_db(void);
};

/**
 * Function that will initialize FontCache object, load database from standard cache directory and find given 
 * font name.
 */
bool font_cache_find(const char *face, Fl_Font &f, Fl_Fontsize &s);

EDELIB_NS_END
#endif

