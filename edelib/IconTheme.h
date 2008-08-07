/*
 * $Id$
 *
 * Icon loader according to given theme
 * Part of edelib.
 * Copyright (c) 2000-2007 EDE Authors.
 *
 * This program is licenced under terms of the
 * GNU General Public Licence version 2 or newer.
 * See COPYING for details.
 */

#ifndef __ICONTHEME_H__
#define __ICONTHEME_H__

#include "econfig.h"
#include "String.h"
#include "List.h"

EDELIB_NS_BEGIN

/**
 * \class IconTheme
 * \brief Loads named icon according to given theme.
 *
 * IconTheme is icon loader that follows <em>Icon Theme Specification</em>
 * from http://www.freedesktop.org and support PNG and XPM icons. This means 
 * that it can recognize icon themes from any desktop environment that
 * follows this specification.
 *
 * Some statements from specification are not added, like:
 *  - recognizing svg icons
 *  - calculating Threshold key (todo for the future)
 *  - reading .icon data for specific icon, as no one use that in 
 *  time of this writing
 *
 * This is sample how to load theme, and, later, find and load
 * icon from it:
 * \code
 *    // this function should be called only once, at
 *    // program startup; the best place is before application
 *    // load gui data, inside main()
 *    IconTheme::init("some-theme-name");
 *
 *    // ...somewhere in the code, when icon is needed
 *    String myicon = IconTheme::get("editor", ICON_SIZE_SMALL, ICON_CONTEXT_APPLICATION);
 *
 *    // ...at the end of programs code, when data is not needed
 *    // any more
 *    IconTheme::shutdown();
 *  \endcode
 *
 * IconTheme::init() <b>must</b> be called before IconTheme::get(), or
 * assertion will trigger. This function will search, check and cache
 * directories that contains given icon theme.
 *
 * If given theme is not found, empty string will be returned when icon
 * is tried to be fetched.
 *
 * IconTheme::get() accepts name of the icon <b>without extension</b>, icon
 * size, and optionally, icon context.
 *
 * \note Is prefered to specify icon context since it will decrease scanning
 * for icon, especially if theme contains a lot of icons.
 *
 * IconTheme::shutdown() <b>must</b> be called when there is no need to
 * lookup for icon inside theme again, or it will leave uncleared allocated data.
 */

/**
 * \enum IconSizes
 * \brief Icon sizes to look for
 */
enum IconSizes {
	ICON_SIZE_TINY         = 16,     ///< 16x16 icons
	ICON_SIZE_SMALL        = 22,     ///< 22x22 icons
	ICON_SIZE_MEDIUM       = 32,     ///< 32x32
	ICON_SIZE_LARGE        = 48,     ///< 48x48
	ICON_SIZE_HUGE         = 64,     ///< 64x64
	ICON_SIZE_ENORMOUS     = 128     ///< 128x128
};

/**
 * \enum IconContext
 * \brief Icon types to look for
 */
enum IconContext {
	ICON_CONTEXT_ANY = 0,             ///< Can be any icon context
	ICON_CONTEXT_ACTION,              ///< Icons representing actions
	ICON_CONTEXT_APPLICATION,         ///< Icons representing applications
	ICON_CONTEXT_DEVICE,              ///< Icons representing devices
	ICON_CONTEXT_FILESYSTEM,          ///< Icons representing objects that are part of system
	ICON_CONTEXT_MIMETYPE,            ///< Icons representing MIME types
	ICON_CONTEXT_STOCK,               ///< Icons representing stock objects (gnome themes usually)
	ICON_CONTEXT_EMBLEM,              ///< Icons representing emblem objects (gnome themes usually)
	ICON_CONTEXT_MISC                 ///< Misc icons (gnome themes usually)
};

#ifndef SKIP_DOCS
struct IconDirInfo {
	String      path;
	int         size;
	IconContext context;
};

#define CACHED_ICONS_SIZE 10

struct IconsCached {
	unsigned int hash;
	IconContext  ctx;
	IconSizes    sz;
	String       path;
};

#endif

class EDELIB_API IconTheme 
{
	private:
		static IconTheme* pinstance;
		bool   fvisited;
		String curr_theme;

		list<String>      theme_dirs;
		list<IconDirInfo> dirlist;

		int cache_ptr;
		IconsCached* icached[CACHED_ICONS_SIZE];
		void cache_append(const char* icon, IconSizes sz, IconContext ctx, const String& path);
		bool cache_lookup(const char* icon, IconSizes sz, IconContext ctx, String& ret);

		IconTheme();
		~IconTheme();

		IconTheme(const IconTheme&);
		IconTheme& operator=(IconTheme&);

		void read_inherits(const char* buff);
		void init_base_dirs(void);

	public:
#ifndef SKIP_DOCS
		String lookup(const char* icon, IconSizes sz, IconContext ctx);
		void copy_known_icons(list<String>& lst, IconSizes sz, IconContext ctx);
		void load_theme(const char* theme);
		void clear_data(void);

		const String& current_theme_name(void) const { return curr_theme; }

		static IconTheme* instance(void);
#endif
		/**
		 * Loads given theme from predefined directories
		 */
		static void init(const char* theme);

		/**
		 * Clear allocated data
		 */
		static void shutdown(void);

		/**
		 * Return true if init() was called, but not shutdown().
		 * Otherwise return false.
		 */
		static bool inited(void);

		/**
		 * Return the name of current used theme
		 */
		static const String& theme_name(void);

		/**
		 * Load another theme, clearing all previous internal
		 * data, including cached one
		 */
		static void load(const char* theme);

		/**
		 * Lookup for icon name
		 * \return full path to icon name with extension
		 * \param icon is icon name <b>without</b> extension
		 * \param sz is desired icon size
		 * \param ctx is desired icon context
		 */
		static String get(const char* icon, IconSizes sz, IconContext ctx = ICON_CONTEXT_ANY);

		/**
		 * Retrieve all icons with given size and context found in loaded theme.
		 * Icons will be with full path
		 *
		 * \param lst is list of icons
		 * \param sz is icon size
		 * \param ctx is icon context
		 */
		static void get_all(list<String>& lst, IconSizes sz, IconContext ctx = ICON_CONTEXT_ANY);
};

EDELIB_NS_END
#endif // __ICONTHEME_H__
