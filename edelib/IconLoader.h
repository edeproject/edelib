/*
 * $Id$
 *
 * Icon loader via IconTheme
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

#ifndef __ICONLOADER_H__
#define __ICONLOADER_H__

#include "IconTheme.h"

class Fl_Shared_Image;
class Fl_Widget;

EDELIB_NS_BEGIN

class IconLoaderItem;

/**
 * \enum IconLoaderOptions
 * \brief Settable options for IconLoader functions
 *
 * \todo Explain these better in IconLoader class, with some examples
 */
enum IconLoaderOptions {
	ICON_LOADER_OPTION_NO_ABSOLUTE_PATH = (1 << 1),  ///< do not see given the path as absolute icon path
	ICON_LOADER_OPTION_NO_WIDGET_REDRAW = (1 << 2),  ///< do not redraw widget
	ICON_LOADER_OPTION_NO_ICON_SCALE    = (1 << 3)   ///< do not scale icon
};

/**
 * \class IconLoader
 * \brief Loads icons with IconTheme
 *
 * IconLoader will initialize icon theme via IconTheme and load given icon name or display it
 * in target widget. It will also cache common requests for faster access or utilize Fl_Shared_Image
 * for direct access to the image object.
 *
 * One of the main goals of IconLoader, besides loading icons in various ways, is to allow dinamically
 * icon pixmaps update on widgets, when icon theme was changed. With this, application would receive
 * a new icon pixmaps without knowing that or without needs to be restarted. To allow this, you should
 * let IconLoader load and set images on widgets (described briefly bellow).
 *
 * Icons are searched and fetched via plain icon name; that name does not contains path nor extension.
 * Using this approach (assuming common names are used), you can easily switch between themes without changing 
 * icon names. So, to load <em>foo.png</em> you should use <em>foo</em> name.
 *
 * Here is the sample how to correctly display icon in Fl_Button object:
 * \code
 *  // ...
 *  IconLoader::init();
 *  // ...
 *  Fl_Button* button = new Fl_Button(...);
 *  IconLoader::set(button, "utilities-terminal", ICON_SIZE_MEDIUM);
 * \endcode
 *
 * With this, <em>utilities-terminal</em> icon with requested size will be searched, and displayed in
 * Fl_Button widget. If this icon wasn't found, a fallback icon will be used for that (by default
 * <em>empty</em> icon is used).
 *
 * Also, with this, IconLoader will track these widgets so when icon theme was changed, they would
 * be appropriately refreshed.
 *
 * IconLoader::init() should not be called if edelib::Window class is used; it will call it automatically.
 *
 * To dinamically change icon theme and reload loaded icons, IconLoader::reload() should be used. This function 
 * will correctly update icon paths and redraw widgets. When Window class is used, it will automatically call 
 * reload() when theme was changed, so there is no need to call it manually.
 */
class IconLoader {
private:
	static IconLoader* pinstance;

	list<IconLoaderItem*> items;
	IconTheme*            curr_theme;

	IconLoader();
	~IconLoader();
	IconLoader(const IconLoader&);
	IconLoader& operator=(const IconLoader&);

	void clear_items(void);
public:
#ifndef SKIP_DOCS
	const char* get_icon_path(const char* name, IconSizes sz, IconContext ctx);
	Fl_Shared_Image* get_icon(const char* name, IconSizes sz, IconContext ctx, unsigned long options);
	bool set_icon(const char* name, Fl_Widget* widget, IconSizes sz, IconContext ctx, unsigned long options);
	void load_theme(const char* name);
	void reload_icons(void);
	void repoll_icons(void);
	const IconTheme* current_theme(void) const { return curr_theme; }

	static IconLoader* instance();
#endif

	/**
	 * Initialize loader code and load given theme name. This function <em>must</em> be called
	 * before further calls
	 */
	static void init(const char* theme);

	/**
	 * Initialize loader code using default theme name (\see IconTheme::default_theme_name())
	 */
	static void init(void) { IconLoader::init(IconTheme::default_theme_name()); }

	/**
	 * Shutdowns loader by cleaning internal data. Call init() to load it again
	 */
	static void shutdown(void);

	/**
	 * Check if data was loaded
	 */
	static bool inited(void);

	/**
	 * Reload IconLoader with the new theme. This function is preferred to load another theme than 
	 * shutdown()/init() combination, since it will not clear all internal data.
	 *
	 * \note This function should not be used when edelib::Window is used for window construcion;
	 * edelib::Window tracks icon theme changes via XSETTINGS protocol and when needed, it will call this
	 * function. Manually calling it (but edelib::Window is used), will change icon theme but XSETTINGS 
	 * data will not be synced, yielding different themes in edelib and XSETTINGS aware toolkits (like Gtk+).
	 */
	static void reload(const char* theme);

	/**
	 * Refresh internal cached icon paths. This function should be used in conjuction with set() or get() functions.
	 * Since IconLoader can't dinamically track life of image objects or widget objects it will not
	 * be able to clear cached paths when these get destroyed. Paths are needed to be kept in case when
	 * icon theme was changed during program execution.
	 *
	 * This function should be used when there are large number of set()/get() and where widgets gets
	 * destroyed in the short period of time. If this wasn't used, internal cache would grow needlessly.
	 */
	static void repoll(void);

	/**
	 * Returns image object by searching icon that matches name, size and context. First, it will check
	 * if <em>name</em> is absolute path to the icon (unless <em>ICON_LOADER_OPTION_NO_ABSOLUTE_PATH</em> 
	 * was used in <em>options</em>) and will try to load it. If fails, it will consult icon theme.
	 *
	 * Returned icon will match <em>sz</em> size; if it is larger or smaller than the loaded, it will be scalled
	 * to the given size. If <em>ICON_LOADER_OPTION_NO_ICON_SCALE</em> was set, scaling will not be done.
	 *
	 * If icon wasn't found, fallback icon will be loaded. If fails, NULL will be returned. For further members 
	 * of Fl_Shared_Image, see FLTK documentation.
	 *
	 * <em>options</em> is used to OR IconLoaderOptions values.
	 */
	static Fl_Shared_Image* get(const char* name, IconSizes sz, IconContext ctx = ICON_CONTEXT_ANY, 
			unsigned long options = 0);

	/**
	 * Returns full path to given icon name. If icon wasn't found, returned string will be empty.
	 */
	static String get_path(const char* name, IconSizes sz, IconContext ctx = ICON_CONTEXT_ANY);

	/**
	 * Load given icon and set widget to use it (it will call widget's image() function for that). This is 
	 * the function you should use to set icon inside the widget, because it allows dynamic refreshing when 
	 * icon theme was changed by redrawing the widget.
	 *
	 * It will try to load icon the sam way as get() does: first it will check if <em>name</em> is
	 * absolute path then will go in icon theme. If this fails, it will try to load fallback icon and if 
	 * succeeded (in one of the cases), it will redraw the widget (unless <em>ICON_LOADER_OPTION_NO_WIDGET_REDRAW</em> 
	 * was OR-ed in <em>options</em>).
	 *
	 * To retrieve the image object of set image, you can use image() function from given widget object.
	 */
	static bool set(Fl_Widget* widget, const char* name, IconSizes sz, IconContext ctx = ICON_CONTEXT_ANY,
			unsigned long options = 0);

	/**
	 * Returns IconTheme object.
	 */
	static const IconTheme* theme(void);

	/**
	 * Set fallback icon name. This name will be used to search alternative icon in icon theme when the target 
	 * one wasn't found. Defauly fallback icon is <em>empty</em>.
	 *
	 * \note icon name will not be copied so make sure to have given string in static memory
	 */
	static void set_fallback_icon(const char* name);

	/**
	 * Return the name of fallback icon. Default is <em>empty</em>.
	 */
	static const char* get_fallback_icon(void);

	/**
	 * Returns XPM icon with given size. These icons are always present in edelib and should be used in
	 * cases when icon is not present in icon theme, or give icon theme wasn't installed at all.
	 */
	static char** get_builtin_xpm_icon(IconSizes sz);
};

EDELIB_NS_END
#endif
