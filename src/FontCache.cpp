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
#include <edelib/String.h>
#include <edelib/Util.h>
#include <edelib/Debug.h>
#include <edelib/Missing.h>
#include <FL/Fl.H>

#include "sdbm/sdbm.h"

/* database version for possible future changes */
#define FONT_CACHE_DB_SCHEMA_VERSION 1.0

/* maximum size for face size with size */
#define FONT_CACHE_FACE_LEN 128

EDELIB_NS_BEGIN

struct FontInfo {
	char face[128];   /* terminated with 0 */
	int  sizes[64];   /* if sizes are less that 64, 0 will be termination number */
	int  nsizes;
	int  type;        /* 0 normal, 1 bold, 2 italic, 3 bold italic */
	int  fltk_font;   /* for future */
};

static bool have_size(FontInfo *fi, int sz) {
	for(int i = 0; i < fi->nsizes; i++) {
		E_DEBUG("!!! %i == %i %i\n", fi->sizes[i], sz, fi->nsizes);
		if(fi->sizes[i] == sz) {
			return true;
		}
	}

	return false;
}

bool font_cache_init(const char *dir, const char *db, const char *prefix) {
	E_RETURN_VAL_IF_FAIL(dir != NULL, false);
	E_RETURN_VAL_IF_FAIL(db != NULL, false);

	String path = dir;
	path.append(E_DIR_SEPARATOR_STR);
	if(prefix) path.append(prefix);
	path.append(E_DIR_SEPARATOR_STR).append(db);

	/* open database first */
	SDBM *fdb = sdbm_open((char*)path.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0640);
	E_RETURN_VAL_IF_FAIL(fdb != NULL, false);

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
		key.dsize = edelib_strnlen(name, 128);

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
	return true;
}

bool font_cache_init(void) {
	String path = user_cache_dir();
	return font_cache_init(path.c_str());
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

bool font_cache_get_by_name(const char *font, Fl_Font &f, Fl_Fontsize &s, const char *dir, const char *db, const char *prefix) {
	E_RETURN_VAL_IF_FAIL(db != NULL, false);
	E_RETURN_VAL_IF_FAIL(font!= NULL, false);
	E_RETURN_VAL_IF_FAIL(dir != NULL, false);

	/* before opening database, parse font to font name and size */
	char face[FONT_CACHE_FACE_LEN];
	int  facesz;
	
	if(!parse_font(font, face, facesz, FONT_CACHE_FACE_LEN))
		return false;

	String path = dir;
	path.append(E_DIR_SEPARATOR_STR);
	if(prefix) path.append(prefix);
	path.append(E_DIR_SEPARATOR_STR).append(db);

	SDBM *fdb = sdbm_open((char*)path.c_str(), O_RDONLY, 0640);
	E_RETURN_VAL_IF_FAIL(fdb != NULL, false);

	datum key, val;

	/* see if we have matching version */
	key.dptr = (char*)"font-cache:version";
	key.dsize = 18;
	val = sdbm_fetch(fdb, key);
	if(!val.dptr) {
		E_WARNING(E_STRLOC ": Unrecognized database format\n");
		sdbm_close(fdb);
		return false;
	}

	double *ver = (double*)val.dptr;
	if(*ver != FONT_CACHE_DB_SCHEMA_VERSION) {
		E_WARNING(E_STRLOC ": Wrong database version\n");
		sdbm_close(fdb);
		return false;
	}

	/* try to find face/size combination */
	key.dptr = (char*)face;
	key.dsize = edelib_strnlen(face, FONT_CACHE_FACE_LEN);

	val = sdbm_fetch(fdb, key);
	if(!val.dptr) {
		E_WARNING(E_STRLOC ": Font face '%s' not found\n", face);
		sdbm_close(fdb);
	   	return false;
	}

	FontInfo *fi = (FontInfo*)val.dptr;
	bool     ret = true;

	if(!have_size(fi, facesz)) {
		E_WARNING(E_STRLOC ": Font size '%i' not found\n", facesz);
		ret = false;
	}

	sdbm_close(fdb);
	return ret;
}

bool font_cache_get_by_name(const char *face, Fl_Font &f, Fl_Fontsize &s) {
	String path = user_cache_dir();
	return font_cache_get_by_name(face, f, s, path.c_str());
}

EDELIB_NS_END
