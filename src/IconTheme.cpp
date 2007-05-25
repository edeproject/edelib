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

EDELIB_NAMESPACE {

IconTheme* IconTheme::pinstance = NULL;

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
const char* icon_extensions[] = {
	".png",
	".xpm"
};

int check_sz(int sz)
{
	if(sz < ICON_SIZE_TINY || sz > ICON_SIZE_ENORMOUS) {
		EWARNING(ESTRLOC ": Unsupported size '%i', defaulting to the 32x32\n", sz);
		return ICON_SIZE_MEDIUM;
	}

	return sz;
}

IconContext figure_ctx(const String& ctx)
{
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

	// gnome addons; 'Emblems' are ignored
	if(ctx == "Stock")
		return ICON_CONTEXT_STOCK;
	if(ctx == "Misc")
		return ICON_CONTEXT_MISC;

	EWARNING(ESTRLOC ": Unknown icon context '%s', defaulting to the ICON_CONTEXT_ANY\n", ctx.c_str());
	return ICON_CONTEXT_ANY;
}


IconTheme::IconTheme() : fvisited(false), curr_theme("")
{
}

IconTheme::~IconTheme() 
{
}

/*
 * Load base directories required from Theme specs
 * in order as noted there. The order is:
 *  1. /home/user/.icons
 *  2. XDG_DATA_DIRS/icons
 *  3. /usr/share/pixmaps
 *  4. rest
 */
void IconTheme::init_base_dirs(void)
{
	theme_dirs.reserve(10);

	// TODO: I will need some make_dir_path() functions somewhere
	String path = dir_home();
	path += "/.icons/";

	theme_dirs.push_back(path);

	vector<String> xdg_data_dirs;
	unsigned int sz = system_data_dirs(xdg_data_dirs);

	if(sz) {
		for(unsigned int i = 0; i < sz; i++) {
			path.clear();
			path = xdg_data_dirs[i];
			path += dir_separator();

			theme_dirs.push_back(path);
		}
	}

	theme_dirs.push_back("/usr/share/pixmaps/");
	theme_dirs.push_back("/opt/kde/share/icons/");
}

/*
void IconTheme::init(void)
{
	if(IconTheme::pinstance != NULL)
		return;
	IconTheme::pinstance = new IconTheme();
}	
*/

void IconTheme::init(const char* theme) 
{
	if(IconTheme::pinstance != NULL)
		return;

	EASSERT(theme != NULL);
	IconTheme::pinstance = new IconTheme();

	IconTheme::pinstance->init_base_dirs();
	IconTheme::pinstance->load_theme(theme);
}

void IconTheme::shutdown(void) 
{
	if(IconTheme::pinstance == NULL)
		return;

	delete IconTheme::pinstance;
	IconTheme::pinstance = NULL;
}

IconTheme* IconTheme::instance(void)
{
	EASSERT(IconTheme::pinstance != NULL && "Did you run IconTheme::init() ?");
	return IconTheme::pinstance;
}

/*
 * If given theme inherits one, all theme_dirs should
 * be prescanned for that theme; that applies too if inherited
 * inherits another, and etc.
 */
void IconTheme::load_theme(const char* theme) 
{
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
	for(unsigned int i = 0; i < theme_dirs.size(); i++) {
		tt = theme_dirs[i].c_str();

		if(dir_exists(tt)) {
			tpath = tt;
			tpath += curr_theme;

			if(dir_exists(tpath.c_str())) {
				tpath += dir_separator();

				ipath = tpath;
				ipath += "index.theme";

				if(file_exists(ipath.c_str())) {
					found = true;
					break;
				}
				EDEBUG(ESTRLOC ": index.theme not found in %s, skipping...\n", ipath.c_str());
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
		EWARNING(ESTRLOC ": %s not accessible\n", ipath.c_str());
		return;
	}

	/*
	 * TODO: this buffer can be small for large Directories values
	 * Some dynamic facility is needed
	 */
	char buffer[3072];
	if(!c.get("Icon Theme", "Directories", buffer, sizeof(buffer))) {
		EWARNING(ESTRLOC ": bad: %s\n", c.strerror());
		return;
	}

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
	vector<String> dl;
	stringtok(dl, buffer, ",");

	int sz = 0;
	IconDirInfo dinfo;
	for(unsigned int i = 0; i < dl.size(); i++) {
		if(!c.get(dl[i].c_str(), "Size", sz, 0))
			EWARNING(ESTRLOC ": Bad entry '%s' in %s, skipping...\n", dl[i].c_str(), ipath.c_str());

		dinfo.size = check_sz(sz);

		if(!c.get(dl[i].c_str(), "Context", buffer, sizeof(buffer)))
			EWARNING(ESTRLOC ": Bad entry '%s' in %s, skipping...\n", dl[i].c_str(), ipath.c_str());

		dinfo.context = figure_ctx(buffer);

		// and finally record the path
		dinfo.path = tpath;
		dinfo.path += dl[i];

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
	if(!c.get("Icon Theme", "Inherits", buffer, sizeof(buffer))) {
		// prevent infinite recursion
		if(!fvisited) {
			EDEBUG(ESTRLOC ": No parents, going for '%s'\n", FALLBACK_THEME);
			fvisited = true;
			load_theme(FALLBACK_THEME);
		}
	}
	else {
		//load_theme(buffer);
		read_inherits(buffer);
	}
}

void IconTheme::read_inherits(const char* buff)
{
	EASSERT(buff != NULL);
	vector<String> parents;
	stringtok(parents, buff, ",");

	for(unsigned int i = 0; i < parents.size(); i++) { 
		str_trim((char*)parents[i].c_str());
		load_theme(parents[i].c_str());
	}
}

void IconTheme::clear_data(void)
{
	/*
	 * This is not called inside load_theme()
	 * so we can recursively collect Inherits key
	 */
	curr_theme = "";
	dirlist.clear();
}

String IconTheme::lookup(const char* icon, IconSizes sz, IconContext ctx)
{
	if(dirlist.size() == 0)
		return "";

	String ret;
	ret.reserve(50);
	/*
	 * ICON_CONTEXT_ANY means context is ignored, but also means slower lookup
	 * since all entries are searched
	 */
	for(unsigned int i = 0; i < dirlist.size(); i++) 
		if(dirlist[i].size == sz && (dirlist[i].context == ctx || ctx == ICON_CONTEXT_ANY)) {
			for(int j = 0; j < ICON_EXT_SIZE; j++) {
				ret = dirlist[i].path;
				ret += dir_separator();
				ret += icon;
				ret += icon_extensions[j];

				if(file_exists(ret.c_str()))
					return ret;
			}
		}

	return "";
}

String IconTheme::get(const char* icon, IconSizes sz, IconContext ctx)
{
	EASSERT(icon != NULL);
	return IconTheme::instance()->lookup(icon, sz, ctx);
}

void IconTheme::load(const char* theme)
{
	EASSERT(theme != NULL);

	IconTheme::instance()->clear_data();
	IconTheme::instance()->load_theme(theme);
}

}
