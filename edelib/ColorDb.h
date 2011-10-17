/*
 * $Id: String.h 2839 2009-09-28 11:36:20Z karijes $
 *
 * X11 color database reader
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

#ifndef __EDELIB_COLORDB_H__
#define __EDELIB_COLORDB_H__

#include "edelib-global.h"

EDELIB_NS_BEGIN

struct ColorDb_P;

/**
 * \class ColorDb
 * \brief X11 color database
 *
 * ColorDb handles X11 color database, usualy stored in <em>/usr/share/X11/rgb.txt</em>. The main intent is to
 * provide human readable color names that will be resolved to associated rgb values.
 */
class EDELIB_API ColorDb {
private:
	ColorDb_P *priv;
	E_DISABLE_CLASS_COPY(ColorDb)
public:
	/** Constructor. */
	ColorDb();

	/** Destructor. */
	~ColorDb();

	/**
	 * Load and parse database file from known locations (<em>/etc/X11/rgb.txt</em> or <em>/usr/share/X11/rgb.txt</em>).
	 * Returns true if loaded something meaningful.
	 */
	bool load(void);

	/**
	 * Explicitly load database from given file. Return false if fails.
	 */
	bool load(const char *file);

	/**
	 * Lookup given name and return RGB triplet. If name wasn't found, it will only return false.
	 */
	bool find(const char *name, unsigned char &r, unsigned char &g, unsigned char &b);
};

EDELIB_NS_END
#endif

