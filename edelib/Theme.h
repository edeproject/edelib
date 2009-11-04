/*
 * $Id$
 *
 * Theming stuff
 * Copyright (c) 2009 edelib authors
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

#ifndef __EDELIB_THEME_H__
#define __EDELIB_THEME_H__

#include "edelib-global.h"

EDELIB_NS_BEGIN

class Theme_P;

/**
 * \ingroup widgets
 *
 * \class Theme
 * \brief Theming engine for widgets
 *
 * Theme class implements abastract theming engine suitable for FLTK and edelib widgets. Unlike Config or 
 * Resource, theme engine implements full language to load and parse theme files giving huge flexibility
 * to represent the final visual appearance and behaviour.
 *
 * This class should not be used directly to load themes; ThemeLoader should be used for that.
 */
class Theme {
private:
	Theme_P *priv;
	E_DISABLE_CLASS_COPY(Theme)
public:
	/** Constructor. */
	Theme();

	/** Destructor. */
	~Theme();

	/**
	 * Load and parse theme from given file. If fails returns false.
	 */
	bool load(const char *f);

	/**
	 * Deinitialize interpreter and clears internal data.
	 */
	void clear(void);

	/**
	 * Return true if theme loaded successfully.
	 */
	bool loaded(void) const;

	/**
	 * Get C string item from theme using <em>style_name</em> style. Item will be stored in <em>ret</em> 
	 * using no more than <em>sz</em> bytes. Return true if found and <em>ret</em> was set.
	 */
	bool get_item(const char *style_name, const char *item_name, char *ret, unsigned int sz);

	/**
	 * Get long item from theme using <em>style_name</em> style. If given item wasn't found, the function
	 * will return false and <em>ret</em> will have <em>fallback</em> value. Otherwise, <em>ret</em> will
	 * have value associated with <em>item_name</em>.
	 */
	bool get_item(const char *style_name, const char *item_name, long &ret, long fallback = 0);

	/** Return author entry. */
	const char *author(void) const;

	/** Return theme name. */
	const char *name(void) const;

	/** Return name of theme example image. */
	const char *sample_image(void) const;
};

EDELIB_NS_END
#endif

