/*
 * $Id$
 *
 * Icon theme
 * Copyright (c) 2005-2009 edelib authors
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

#ifndef __ICONTHEME_H__
#define __ICONTHEME_H__

#include "String.h"
#include "List.h"

EDELIB_NS_BEGIN

/**
 * \enum IconSizes
 * \brief Icon sizes to look for
 */
enum IconSizes {
	ICON_SIZE_TINY         = 16,   ///< 16x16 icons
	ICON_SIZE_SMALL        = 22,   ///< 22x22 icons
	ICON_SIZE_MEDIUM       = 32,   ///< 32x32
	ICON_SIZE_LARGE        = 48,   ///< 48x48
	ICON_SIZE_HUGE         = 64,   ///< 64x64
	ICON_SIZE_ENORMOUS     = 128   ///< 128x128
};

/**
 * \enum IconContext
 * \brief Icon types to look for
 */
enum IconContext {
	ICON_CONTEXT_ANY = 0,          ///< Can be any icon context
	ICON_CONTEXT_ACTION,           ///< Icons representing actions
	ICON_CONTEXT_APPLICATION,      ///< Icons representing applications
	ICON_CONTEXT_DEVICE,           ///< Icons representing devices
	ICON_CONTEXT_FILESYSTEM,       ///< Icons representing objects that are part of system
	ICON_CONTEXT_MIMETYPE,         ///< Icons representing MIME types
	ICON_CONTEXT_STOCK,            ///< Icons representing stock objects (gnome themes usually)
	ICON_CONTEXT_EMBLEM,           ///< Icons representing emblem objects (gnome themes usually)
	ICON_CONTEXT_MISC              ///< Misc icons (gnome themes usually)
};

class IconThemePrivate;

/**
 * \class IconTheme
 * \brief Finds named icon according to the given theme
 *
 * IconTheme is icon finder via <em>Icon Theme Specification</em> from http://www.freedesktop.org. 
 * This specification prescribes how icons should be located when icon name was given in desktop neutral 
 * way. With this, all common desktop environments and apps could share icons and themes.
 *
 * IconTheme closely follows this specification, with a few minor exceptions:
 *  - recognizing SVG icons, since edelib currently does not have any SVG support
 *  - calculating Threshold key (todo for the future)
 *  - reading .icon data for specific icon, as no one use that in the time of this writing
 *
 * Icons are searched by giving the icon name, without extension, and IconTheme will try to find
 * either PNG or XPM icon with the same name.
 *
 * Although this class can be used directly, preferred way is to load icons via IconLoader.
 *
 * \todo implement Threshold support (see icon-theme spec)
 */
class EDELIB_API IconTheme {
private:
	IconThemePrivate* priv;

	void load_theme(const char* name);
	void read_inherits(const char* buf);

	E_DISABLE_CLASS_COPY(IconTheme)
public:
	/**
	 * Empty constructor
	 */
	IconTheme() : priv(NULL) { }

	/**
	 * Destructor, calls clear()
	 */
	~IconTheme() { clear(); }

	/**
	 * Load theme. Must be called before icons search. Calling load() again with
	 * the new theme name will initialize that new theme
	 */
	void load(const char* name);

	/**
	 * Unload current theme and clear allocated data
	 */
	void clear(void);

	/**
	 * Return full path to the icon name. If icon wasn't found, it will return empty string
	 */
	String find_icon(const char* icon, IconSizes size, IconContext context = ICON_CONTEXT_ANY);

	/**
	 * Returns current given theme name, or NULL if theme wasn't loaded via load()
	 */
	const char* theme_name(void) const;

	/**
	 * Returns current theme name. The difference between this function and theme_name() is that
	 * theme_name() represents what was given to load() function, but stylized_theme_name() is the value
	 * of the <em>Name</em> key in index.theme file.
	 *
	 * <em>Name</em> is often localized so it should be used to publicly present icon theme name
	 */
	const char* stylized_theme_name(void) const;

	/**
	 * Returns description of loaded icon theme. If description wasn't found, returned string will be NULL
	 */
	const char* description(void) const;

	/**
	 * Returns the name of example icon or NULL if wasn't found. Example icon is value of <em>Example</em>
	 * key and is plain name that could be used with find_icon() to search it's full path
	 */
	const char* example_icon(void) const;

	/**
	 * Query available icons from loaded theme and inherited themes. Icons will have full path
	 */
	void query_icons(list<String>& lst, IconSizes size, IconContext context = ICON_CONTEXT_ANY) const;

	/**
	 * Returns name for default icon theme
	 */
	static const char* default_theme_name(void) { return "edeneu"; }
};

EDELIB_NS_END
#endif // __ICONTHEME_H__
