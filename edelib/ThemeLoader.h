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

#ifndef __EDELIB_THEMELOADER_H__
#define __EDELIB_THEMELOADER_H__

#include "Theme.h"
#include "XSettingsClient.h"

EDELIB_NS_BEGIN

struct ThemeLoader_P;

/**
 * \ingroup widgets
 *
 * \class ThemeLoader
 * \brief Themes loader
 *
 * ThemeLoader is a class for loading themes and applying theme content on currently available FLTK tree.
 * It will search for themes in <i>$XDG_DATA_DIRS/prefix/theme-name</i> and try to load <i>main.ewt</i>
 * as the entry point and interpret it via Theme engine class.
 *
 * For more details see \ref themedoc.
 *
 * Also, ThemeLoader will load client for XSETTINGS protocol (via XSettingsClient), giving the single place
 * for accessing common visual elements either using theme files or XSETTINGS protocol.
 */
class EDELIB_API ThemeLoader {
private:
	ThemeLoader_P *priv;

	void apply_common_gui_elements(void);
	E_DISABLE_CLASS_COPY(ThemeLoader)
public:
	/** Constructor. */
	ThemeLoader();

	/** Destructor. Cleans initialized data. */
	~ThemeLoader();

	/**
	 * Loads theme using XDG_DATA_DIRS location. The final location is constructed adding prefix and
	 * default theme name, which by default looks as <i>$XDG_DATA_DIRS/ede/default</i>. 
	 *
	 * As theme name is at the same time also a directory with theming files, loader will try first to
	 * look for <i>main.ewt</i> file in this directory. If it is unable to find it nor to interpret it
	 * correctly, it will return false.
	 */
	bool load(const char *name = "default", const char *prefix = "ede");

	/**
	 * Load theme using given path.
	 */
	bool load_with_path(const char *path);

	/**
	 * Loads XSETTINGS client code. It is assumed how display was opened (by using <i>fl_open_display()</i>)
	 * before or it will return false. Also, it will return false if XSETTINGS client code fails to load.
	 */
	bool load_xsettings(void);

	/**
	 * Returns currently loaded theme. If theme wasn't loaded successfully, it will return NULL.
	 */
	Theme *theme(void);

	/**
	 * Returns XSettingsClient object. If wasn't loaded successfully with load_xsettings(), it will return NULL.
	 */
	XSettingsClient *xsettings(void);

#ifndef SKIP_DOCS
	E_CLASS_GLOBAL_EXPLICIT_DECLARE(ThemeLoader)
#endif
};

EDELIB_NS_END
#endif

