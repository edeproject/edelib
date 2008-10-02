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


#include <edelib/IconTheme.h>
#include <edelib/Debug.h>
#include <edelib/Directory.h>
#include <edelib/File.h>
#include <edelib/Config.h>
#include <edelib/StrUtil.h>
#include <edelib/Util.h>
#include <stdio.h>
#include <string.h> // strlen

EDELIB_NS_BEGIN

IconTheme* IconTheme::pinstance = NULL;

typedef list<String>                StringList;
typedef list<String>::iterator      StringListIter;
typedef list<IconDirInfo>::iterator DirListIter;

/*
 * Mandatory alternative theme if icon
 * is not found in specified one
 */
#define FALLBACK_THEME "hicolor"

/*
 * Icon Theme Specification adds .svg too, but
 * we do not support them for now
 */
#define ICON_EXT_SIZE 2
static const char* icon_extensions[] = {
	".png",
	".xpm"
};

static void append_to_list(StringList& from, StringList& to) {
	StringListIter it = from.begin(), it_end = from.end();
	for(; it != it_end; ++it)
		to.push_back(*it);
}

static unsigned int do_hash(const char* key, int keylen) {
	unsigned hash ;
	int i;
	for (i = 0, hash = 0; i < keylen ;i++) {
		hash += (long)key[i] ;
		hash += (hash<<10);
		hash ^= (hash>>6) ;
	}
	hash += (hash <<3);
	hash ^= (hash >>11);
	hash += (hash <<15);
	return hash ;
}

static int check_sz(int sz) {
	if(sz < ICON_SIZE_TINY || sz > ICON_SIZE_ENORMOUS) {
		E_WARNING(E_STRLOC ": Unsupported size '%i', defaulting to the 32x32\n", sz);
		return ICON_SIZE_MEDIUM;
	}

	return sz;
}

static IconContext figure_ctx(const String& ctx) {
	// mandatory ones
	if(ctx == "Actions")
		return ICON_CONTEXT_ACTION;
	if(ctx == "Devices")
		return ICON_CONTEXT_DEVICE;
	if(ctx == "FileSystems")
		return ICON_CONTEXT_FILESYSTEM;
	if(ctx == "MimeTypes")
		return ICON_CONTEXT_MIMETYPE;

	// kde addons
	if(ctx == "Applications")
		return ICON_CONTEXT_APPLICATION;

	if(ctx == "Stock")
		return ICON_CONTEXT_STOCK;
	if(ctx == "Emblems")
		return ICON_CONTEXT_EMBLEM;
	if(ctx == "Misc")
		return ICON_CONTEXT_MISC;

	E_WARNING(E_STRLOC ": Unknown icon context '%s', defaulting to the ICON_CONTEXT_ANY\n", ctx.c_str());
	return ICON_CONTEXT_ANY;
}


IconTheme::IconTheme() : fvisited(false), curr_theme("") {
	for(int i = 0; i < CACHED_ICONS_SIZE; i++)
		icached[i] = NULL;

	cache_ptr = 0;
}

IconTheme::~IconTheme() {
	for(int i = 0; i < CACHED_ICONS_SIZE; i++) {
		delete icached[i];
		icached[i] = NULL;
	}

	E_DEBUG(E_STRLOC " : IconTheme::~IconTheme() cache dismiss\n");
}

void IconTheme::cache_append(const char* icon, IconSizes sz, IconContext ctx, const String& path) {
	unsigned long hash = do_hash(icon, strlen(icon));
	hash += sz;
	hash += ctx;

	if(cache_ptr == CACHED_ICONS_SIZE) {
		cache_ptr = 0;
		E_DEBUG(E_STRLOC ": IconTheme cache rollower\n");
	}

	if(icached[cache_ptr]) {
		delete icached[cache_ptr];
		icached[cache_ptr] = NULL;
	}

	icached[cache_ptr] = new IconsCached;
	icached[cache_ptr]->hash = hash;
	icached[cache_ptr]->path = path;
	icached[cache_ptr]->sz   = sz;
	icached[cache_ptr]->ctx  = ctx;

	cache_ptr++;
}

bool IconTheme::cache_lookup(const char* icon, IconSizes sz, IconContext ctx, String& ret) {
	unsigned long hash = do_hash(icon, strlen(icon));
	hash += sz;
	hash += ctx;

	for(int i = 0; i < cache_ptr && i < CACHED_ICONS_SIZE; i++) {
		if(icached[i]->hash == hash && icached[i]->sz == sz && icached[i]->ctx == ctx) {
			E_DEBUG(E_STRLOC ": IconTheme::cache_lookup() : cache hit !\n");
			ret = icached[i]->path;
			return true;
		}
	}

	E_DEBUG(E_STRLOC ": IconTheme::cache_lookup() : cache miss !!!\n");
	return false;
}

/*
 * Load base directories required from Theme specs
 * in order as noted there. The order is:
 *  1. /home/user/.icons
 *  2. XDG_DATA_DIRS/icons
 *  3. /usr/share/pixmaps
 *  4. rest
 */
void IconTheme::init_base_dirs(void) {
	String path = dir_home();
	path += "/.icons/";

	theme_dirs.push_back(path);

	StringList xdg_data_dirs;
	unsigned int sz = system_data_dirs(xdg_data_dirs);

	if(sz) {
		for(StringListIter it = xdg_data_dirs.begin(); it != xdg_data_dirs.end(); ++it) {
			path.clear();
			path = (*it);
			path += "/icons/";

			theme_dirs.push_back(path);
		}
	}

	theme_dirs.push_back("/usr/share/pixmaps/");
	theme_dirs.push_back("/opt/kde/share/icons/");
}

void IconTheme::init(const char* theme) {
	if(IconTheme::pinstance != NULL)
		return;

	E_ASSERT(theme != NULL);
	IconTheme::pinstance = new IconTheme();

	IconTheme::pinstance->init_base_dirs();
	IconTheme::pinstance->load_theme(theme);
}

void IconTheme::shutdown(void) {
	if(IconTheme::pinstance == NULL)
		return;

	delete IconTheme::pinstance;
	IconTheme::pinstance = NULL;
}

bool IconTheme::inited(void) {
	return (IconTheme::pinstance != NULL);
}

IconTheme* IconTheme::instance(void) {
	E_ASSERT(IconTheme::pinstance != NULL && "Did you run IconTheme::init() ?");
	return IconTheme::pinstance;
}

/*
 * If given theme inherits one, all theme_dirs should
 * be prescanned for that theme; that applies too if inherited
 * inherits another, and etc.
 */
void IconTheme::load_theme(const char* theme) {
	curr_theme = theme;

	String tpath;
	String ipath;
	bool found = false;

	/*
	 * Lookup throught the icon_directories + theme name;
	 * if found, try to locate index.theme since is
	 * sign for theme directory
	 */
	const char* tt;
	for(StringListIter it = theme_dirs.begin(); it != theme_dirs.end(); ++it) {
		tt = (*it).c_str();

		if(dir_exists(tt)) {
			tpath = tt;
			tpath += curr_theme;

			if(dir_exists(tpath.c_str())) {
				tpath += E_DIR_SEPARATOR_STR;

				ipath = tpath;
				ipath += "index.theme";

				if(file_exists(ipath.c_str())) {
					found = true;
					break;
				}
				E_DEBUG(E_STRLOC ": index.theme not found in %s, skipping...\n", ipath.c_str());
			}
		}
	}

	if(!found)
		return;

	/*
	 * Now open index.theme and read 'Directories' key;
	 * this key contains in which subdirectories we should
	 * look for icons and their sizes
	 */
	Config c;
	if(!c.load(ipath.c_str())) {
		E_WARNING(E_STRLOC ": %s not accessible\n", ipath.c_str());
		return;
	}

	char* buffer = NULL;
	unsigned int bufflen;

	if(!c.get_allocated("Icon Theme", "Directories", &buffer, bufflen)) {
		E_WARNING(E_STRLOC ": bad: %s\n", c.strerror());
		return;
	}

	E_ASSERT(buffer != NULL);

	// used for Context/Inherits
	char static_buffer[1024];

	/*
	 * The format of Directories key is: 'Directories = size1/type, size2/type, size3/type, etc.'
	 * Here is first splitted by ',' to get a list of keys that should be below Directories key;
	 * they usually have form:
	 *
	 * [size/type]
	 *   Context = xxx
	 *   Type = xxx
	 *   Threshold = xxx
	 *
	 * For now Threshold is ignored
	 */
	StringList dl;
	stringtok(dl, buffer, ",");
	delete [] buffer;

	int sz = 0;
	IconDirInfo dinfo;
	for(StringListIter it = dl.begin(); it != dl.end(); ++it) {
		// remove spaces
		str_trim((char*)(*it).c_str());

		if(!c.get((*it).c_str(), "Size", sz, 0))
			E_WARNING(E_STRLOC ": Bad entry '%s' in %s, skipping...\n", (*it).c_str(), ipath.c_str());

		dinfo.size = check_sz(sz);

		if(!c.get((*it).c_str(), "Context", static_buffer, sizeof(static_buffer)))
			E_WARNING(E_STRLOC ": Bad entry '%s' in %s, skipping...\n", (*it).c_str(), ipath.c_str());

		dinfo.context = figure_ctx(static_buffer);

		// and finally record the path
		dinfo.path = tpath;
		dinfo.path += (*it);

		dirlist.push_back(dinfo);
	}
	
	/*
	 * Now try to read Inherits key which represent parents; if
	 * not found, default should be 'hicolor';
	 * this key can have multiple values, like:
	 *
	 * Inherits = theme1,theme2
	 *
	 * They all must be scanned :(
	 */
	if(!c.get("Icon Theme", "Inherits", static_buffer, sizeof(static_buffer))) {
		// prevent infinite recursion
		if(!fvisited) {
			E_DEBUG(E_STRLOC ": No parents, going for '%s'\n", FALLBACK_THEME);
			fvisited = true;
			load_theme(FALLBACK_THEME);
		}
	}
	else {
		read_inherits(static_buffer);
	}
}

void IconTheme::read_inherits(const char* buff) {
	E_ASSERT(buff != NULL);

	StringList parents;
	stringtok(parents, buff, ",");

	for(StringListIter it = parents.begin(); it != parents.end(); ++it) { 
		// remove spaces
		str_trim((char*)(*it).c_str());

		load_theme((*it).c_str());
	}
}

void IconTheme::clear_data(void) {
	/*
	 * This is not called inside load_theme()
	 * so we can recursively collect Inherits key
	 */
	curr_theme = "";
	dirlist.clear();
}

String IconTheme::lookup(const char* icon, IconSizes sz, IconContext ctx) {
	if(dirlist.size() == 0)
		return "";

	String ret;
	ret.reserve(50);

	if(cache_lookup(icon, sz, ctx, ret))
		return ret;

	/*
	 * ICON_CONTEXT_ANY means context is ignored, but also means slower lookup
	 * since all entries are searched
	 */
	for(DirListIter it = dirlist.begin(); it != dirlist.end(); ++it) 
		if((*it).size == sz && ((*it).context == ctx || ctx == ICON_CONTEXT_ANY)) {
			for(int j = 0; j < ICON_EXT_SIZE; j++) {
				ret = (*it).path;
				ret += E_DIR_SEPARATOR_STR;
				ret += icon;
				ret += icon_extensions[j];

				if(file_exists(ret.c_str())) {
					cache_append(icon, sz, ctx, ret);
					return ret;
				}
			}
		}

	return "";
}

void IconTheme::copy_known_icons(list<String>& lst, IconSizes sz, IconContext ctx) {
	if(dirlist.size() == 0)
		return;

	String icon;
	icon.reserve(50);

	StringList content;

	DirListIter it = dirlist.begin(), it_end = dirlist.end();
	for(; it != it_end; ++it) {
		if((*it).size == sz && ((*it).context == ctx || ctx == ICON_CONTEXT_ANY)) {
			if(dir_list((*it).path.c_str(), content, true))
				append_to_list(content, lst);
		}
	}
}

const String& IconTheme::theme_name(void) {
	return IconTheme::instance()->current_theme_name();
}

void IconTheme::load(const char* theme) {
	E_ASSERT(theme != NULL);

	IconTheme::instance()->clear_data();
	IconTheme::instance()->load_theme(theme);
}

String IconTheme::get(const char* icon, IconSizes sz, IconContext ctx) {
	E_ASSERT(icon != NULL);
	return IconTheme::instance()->lookup(icon, sz, ctx);
}

void IconTheme::get_all(list<String>& lst, IconSizes sz, IconContext ctx) {
	return IconTheme::instance()->copy_known_icons(lst, sz, ctx);
}

EDELIB_NS_END
