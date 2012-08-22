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

/* make it visible outside namespace */
struct scheme;

EDELIB_NS_BEGIN
class Theme_P;

#ifndef SKIP_DOCS
typedef void (*ThemeErrorHandler)(const char *str, void *data);
#endif

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
class EDELIB_API Theme {
private:
	Theme_P           *priv;
	E_DISABLE_CLASS_COPY(Theme)

	/* safe to be called multiple times */
	void init_interpreter(void);
public:
	/** 
	 * Constructor.
	 * \param msg_domain is used for translatable strings and should be set to application name that uses Theme class.
	 */
	Theme(const char *msg_domain = "edelib");

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
	 * Install error handler that will be called when error occured during reading/parsing phase.
	 * Handler should be installed before <em>load()</em> call. When <em>load()</em> completes and return status,
	 * handler function will not be called any more, so all collected errors could be displayed or processed further.
	 */
	void set_error_handler(ThemeErrorHandler func, void *data = 0);

	/** Return error handler data. */
	void *error_handler_data(void) const;

	/** Return error handler or NULL if not set. */
	ThemeErrorHandler error_handler(void) const;

	/**
	 * Show prompt and wait for input. This function is intended to be called from console progams, as it will
	 * wait intfinitely for user input, where scheme/theme expressions could be evaluated.
	 *
	 * This function is mostly for writing external tools for easier debugging.
	 */
	void prompt(void);

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

	/**
	 * Return interpreter instance and can return NULL if theme isn't loaded successfully.
	 * This function makes sense only if you loaded some theme.
	 */
	scheme* get_interpreter(void);
};

EDELIB_NS_END
#endif
