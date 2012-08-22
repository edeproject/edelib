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
 * \def EDELIB_FONT_CACHE_FACE_LEN
 * Maximum number of bytes allowed in font face name.
 */
#define EDELIB_FONT_CACHE_FACE_LEN 64

/**
 * \ingroup widgets
 * \class FontInfo
 * \brief Base structure for storing font information; used by FontCache
 *
 * FontInfo is structure used to store information in cache database. Database is represented in key/value form
 * (actualy it is sdbm powered) where each key is font name and each value FontInfo structure.
 */
struct EDELIB_API FontInfo {
	/** Face name with encoded style; usable only for FLTK. */
	char face[EDELIB_FONT_CACHE_FACE_LEN];
	/** All available sizes for this font. */
	int  sizes[64];
	/** Actual number of sizes in <em>sizes</em> array. */
	int  nsizes;
	/** Type of font; coresponds to FLTK nomenclature (FL_NORMAL, FL_BOLD, FL_ITALIC and FL_BOLD_ITALIC). */
	int  type;
};

/**
 * \ingroup widgets
 * \class FontCache
 * \brief Allow readable font names and cache their access
 *
 * FLTK does not provide any kind of facility for advanced font handling, and this class is to provide
 * at least some basis for this. Each font in FLTK can only be accessed by it's index number, stored in internal
 * font array and set by <em>labelfont()</em>. This scheme is inefficient for non standard fonts (not predefined inside FLTK) as 
 * index number can be different each time application starts, causing high probability to select different font face.
 *
 * FontCache tries to fix this; all fonts are prepopulated (either with <em>FontCache::init_db()</em> or external tool like
 * <em>edelib-update-font-cache</em>), where cache database will be stored in XDG cache directory (\see user_cache_dir()).
 *
 * \todo complete
 */
class EDELIB_API FontCache {
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

	/** Return number of fonts inside database. If database is not loaded or is unable to get font number, return -1. */
	int count(void) const;

	/**
	 * Try to find given face and size in given database path. If found, register it as FLTK font and set font id
	 * and size.
 	 */
	bool find(const char *n, Fl_Font &font, int &size);

	/**
	 * This function can be used to iterate all fonts, where on each font will be called callback. Fonts will not be
	 * in sorted order.
	 */
	void for_each_font(void (*) (const char *n, FontInfo *, void *), void *data = NULL);

	/**
	 * Iterate over fonts, just as <em>for_each_font()</em>, except fonts will be sorted. To operate like this, all
	 * database keys must be fetched first and them sorted in memory. After that, it will access to database content.
	 *
	 * This function is clearly inefficient if called on large database.
	 */
	void for_each_font_sorted(void (*) (const char *n, FontInfo *, void *), void *data = NULL);

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
 * \ingroup widgets
 * Function that will initialize FontCache object, load database from standard cache directory and find given 
 * font name. If fails, it will set default values.
 */
EDELIB_API bool font_cache_find(const char *face, Fl_Font &f, int &s, Fl_Font df = FL_HELVETICA, int ds = 12);

EDELIB_NS_END
#endif

