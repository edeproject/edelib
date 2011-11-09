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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <edelib/FontCache.h>
#include <edelib/Directory.h>
#include <edelib/StrUtil.h>
#include <edelib/String.h>
#include <edelib/List.h>
#include <edelib/Util.h>
#include <edelib/Debug.h>
#include <edelib/Missing.h>
#include <FL/Fl.H>

#include "sdbm/sdbm.h"

/* database version for possible future changes */
#define FONT_CACHE_DB_SCHEMA_VERSION 1.0

/* maximum size for face size with size */
#define FONT_CACHE_FACE_LEN_WITH_SIZE 68

/* maximum size for face */
#define FONT_CACHE_FACE_LEN 64

#define SDBM_SAFE_CLOSE(db) \
	if(db) {                \
		sdbm_close(db);     \
		db = NULL;          \
	}                       \

EDELIB_NS_BEGIN

typedef list<String*>           StrList;
typedef list<String*>::iterator StrListIt;

/* class to allow static storage of font names */
class FontHolder {
private:
	StrList fonts;
public:
	~FontHolder() {
		for(StrListIt it = fonts.begin(), ite = fonts.end(); it != ite; ++it)
			delete *it;
		fonts.clear();
	}

	const char *append(const char *n) {
		String *s = new String(n);
		s->trim();
		fonts.push_back(s);
		return s->c_str();
	}

	unsigned int size(void) const { return fonts.size(); }
};

/* internal holder for registered name inside FLTK */
static FontHolder static_font_names;

struct FontInfo {
	char face[FONT_CACHE_FACE_LEN];   /* terminated with 0 */
	int  sizes[64];                   /* if sizes are less that 64, 0 will be termination number */
	int  nsizes;
	int  type;                        /* 0 normal, 1 bold, 2 italic, 3 bold italic */
};

struct FontCache_P {
	SDBM *db;
	int   count;
};

static bool have_size(FontInfo *fi, int sz) {
	for(int i = 0; i < fi->nsizes; i++) {
		if(fi->sizes[i] == sz)
			return true;
	}

	return false;
}

static bool parse_font(const char *font, char *ret, int &sz, int maxlen) {
	int len = edelib_strnlen(font, maxlen);
	E_RETURN_VAL_IF_FAIL(len > 0, false);

	char nbuf[16];
	char *pos = (char*)font + len - 1;
	int  i;

	for(i = len; *pos >= '0' && *pos <= '9' && i >= 0; pos--, i--)
		;

	/* we don't have font size; default it to 12 */
	if(i == len) {
		E_WARNING(E_STRLOC ": Missing font size; setting default to '12'\n");
		edelib_strlcpy(nbuf, "12", sizeof(nbuf));
	} else {
		edelib_strlcpy(nbuf, pos + 1, sizeof(nbuf));
		len = pos - font;
	}

	/* include space for '\0' */
	len++;

	sz = atoi(nbuf);
	edelib_strlcpy(ret, font, len);

	return true;
}

bool FontCache::load(const char *dir, const char *db, const char *prefix) {
	E_RETURN_VAL_IF_FAIL(dir != NULL, false);
	E_RETURN_VAL_IF_FAIL(db != NULL, false);

	/* do not load already loaded */
	if(priv && priv->db) return true;

	String path = dir;
	path.append(E_DIR_SEPARATOR_STR);
	if(prefix) path.append(prefix);
	path.append(E_DIR_SEPARATOR_STR).append(db);

	/* create on demand */
	if(!priv) {
		priv = new FontCache_P;
		priv->db = NULL;
		priv->count = 0;
	}

	priv->db = sdbm_open((char*)path.c_str(), O_RDONLY, 0640);
	if(!priv->db) return false;

	/* see if we have valid database */
	datum key, val;

	key.dptr = (char*)"font-cache:version";
	key.dsize = 18;
	val = sdbm_fetch(priv->db, key);
	if(!val.dptr) {
		E_WARNING(E_STRLOC ": Unrecognized database format\n");
		SDBM_SAFE_CLOSE(priv->db);
		return false;
	}

	double *ver = (double*)val.dptr;
	if(*ver != FONT_CACHE_DB_SCHEMA_VERSION) {
		E_WARNING(E_STRLOC ": Wrong database version\n");
		SDBM_SAFE_CLOSE(priv->db);
		return false;
	}

	return true;
}

bool FontCache::load(void) {
	String path = user_cache_dir();
	return load(path.c_str());
}

void FontCache::clear(void) {
	E_RETURN_IF_FAIL(priv != NULL);

	SDBM_SAFE_CLOSE(priv->db);
	delete priv;
	priv = NULL;
}

bool FontCache::find(const char *n, Fl_Font &font, int &font_size) {
	E_RETURN_VAL_IF_FAIL(priv->db != NULL, false);
	E_RETURN_VAL_IF_FAIL(n != NULL, false);

	char face[FONT_CACHE_FACE_LEN];
	int  facesz;
	
	if(!parse_font(n, face, facesz, FONT_CACHE_FACE_LEN_WITH_SIZE)) {
		E_WARNING(E_STRLOC ": Unable to parse '%s' as font\n", n);
		return false;
	}

	/* ignore case for font name */
	str_tolower((unsigned char*)face);

	/* find face/size combination */
	datum key, val;

	key.dptr = (char*)face;
	key.dsize = edelib_strnlen(face, FONT_CACHE_FACE_LEN);

	val = sdbm_fetch(priv->db, key);
	if(!val.dptr) {
		E_WARNING(E_STRLOC ": Font face '%s' not found\n", face);
	   	return false;
	}

	FontInfo *fi = (FontInfo*)val.dptr;

	if(!have_size(fi, facesz)) {
		E_WARNING(E_STRLOC ": Font size '%i' not found\n", facesz);
		return false;
	} 
	
	const char *sf = static_font_names.append(fi->face);
	font = (Fl_Font)FL_FREE_FONT + static_font_names.size();
	font_size = facesz;

	/* register it under this index */
	Fl::set_font(font, sf);
	return true;
}

int FontCache::init_db(const char *dir, const char *db, const char *prefix) {
	E_RETURN_VAL_IF_FAIL(dir != NULL, -1);
	E_RETURN_VAL_IF_FAIL(db != NULL, -1);

	String path = dir;
	path.append(E_DIR_SEPARATOR_STR);
	if(prefix) path.append(prefix);

	/* create full path if needed */
	dir_create_with_parents(path.c_str());

	path.append(E_DIR_SEPARATOR_STR).append(db);

	/* open database first */
	SDBM *fdb = sdbm_open((char*)path.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0640);
	E_RETURN_VAL_IF_FAIL(fdb != NULL, -1);

	const char *n, *f;
	char       name[FONT_CACHE_FACE_LEN];
	int        count, type, nsizes, *sizes, nfonts = 0;
	datum      key, val;
	FontInfo   fi;

	/* now register all fonts */
	count = Fl::set_fonts("-*");

	for(int i = 0; i < count; i++) {
		n = Fl::get_font_name((Fl_Font)i, &type);
		if(!n) continue;

		f = Fl::get_font((Fl_Font)i);
		nsizes = Fl::get_font_sizes((Fl_Font)i, sizes);
		if(!nsizes) continue;

		edelib_strlcpy(name, n, FONT_CACHE_FACE_LEN);
		edelib_strlcpy(fi.face, f, FONT_CACHE_FACE_LEN);

		/* ignore case for font name */
		str_tolower((unsigned char*)name);

		/* get sizes */
		if(sizes[0] == 0) {
			/* many sizes; 64 is seen as 1-64 font size range and is limited by FLTK */
			fi.nsizes = 64;
			for(int j = 0; j < fi.nsizes; j++)
				fi.sizes[j] = j + 1;
		} else {
			fi.nsizes = nsizes;
			for(int j = 0; j < fi.nsizes; j++)
				fi.sizes[j] = sizes[j];
		}

		fi.type = type;

		key.dptr = name;
		key.dsize = edelib_strnlen(name, FONT_CACHE_FACE_LEN);

		val.dptr = (char*)&fi;
		val.dsize = sizeof(fi);

		sdbm_store(fdb, key, val, SDBM_REPLACE);
		nfonts++;
	}

	/* store number of records in 'count' key */
	key.dptr = (char*)"font-cache:count";
	key.dsize = 16;

	val.dptr = (char*)&nfonts;
	val.dsize = sizeof(int);
	sdbm_store(fdb, key, val, SDBM_REPLACE);
	
	/* store version */
	key.dptr = (char*)"font-cache:version";
	key.dsize = 18;

	double version = FONT_CACHE_DB_SCHEMA_VERSION;
	val.dptr = (char*)&version;
	val.dsize = sizeof(double);
	sdbm_store(fdb, key, val, SDBM_REPLACE);

	sdbm_close(fdb);
	return nfonts;
}

int FontCache::init_db(void) {
	String path = user_cache_dir();
	return FontCache::init_db(path.c_str());
}

bool font_cache_find(const char *face, Fl_Font &f, int &s, Fl_Font df, int ds) {
	FontCache fc;

	if(fc.load() && fc.find(face, f, s))
		return true;

	f = df;
	s = ds;
	return false;
}

EDELIB_NS_END
