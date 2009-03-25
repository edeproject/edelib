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

#include <edelib/IconTheme.h>
#include <edelib/Config.h>
#include <edelib/File.h>
#include <edelib/Util.h>
#include <edelib/StrUtil.h>
#include <edelib/Directory.h>

EDELIB_NS_BEGIN

/* Mandatory alternative theme if icon is not found in specified one */
#define FALLBACK_THEME "hicolor"

/* * Icon Theme Specification adds .svg too, but we do not support them for now */
static const char* icon_extensions[] = {
	".png",
	".xpm",
	0
};

struct IconDirInfo {
	String      path;
	int         size;
	IconContext context;
};

typedef list<String>                StrList;
typedef list<String>::iterator      StrListIter;

typedef list<IconDirInfo>           DirList;
typedef list<IconDirInfo>::iterator DirListIter;

struct IconThemePrivate {
	bool    fallback_visited;
	bool    info_loaded;
	String  curr_theme;
	String  curr_theme_stylized;
	String  curr_theme_descr;
	String  curr_theme_example;
	StrList theme_dirs;
	DirList dirlist;
};

static void list_append(StrList& from, StrList& to) {
	StrListIter it = from.begin(), it_end = from.end();

	for(; it != it_end; ++it)
		to.push_back(*it);
}

/*
 * Load base directories required from Theme specs in order as noted there. The order is:
 *  1. $HOME/.icons
 *  2. XDG_DATA_DIRS/icons
 *  3. /usr/share/pixmaps
 *  4. rest
 *
 * Note: init_base_dirs() should return paths ending with slashes
 */
static void init_base_dirs(StrList& dirs) {
	String path = dir_home();
	path += "/.icons/";
	dirs.push_back(path);

	path = user_data_dir();
	path += "/icons/";
	dirs.push_back(path);

	StrList xdg_data_dirs;
	system_data_dirs(xdg_data_dirs);

	StrListIter it = xdg_data_dirs.begin(), it_end = xdg_data_dirs.end();

	for(; it != it_end; ++it) {
		path = *it;
		path += "/icons/";
		dirs.push_back(path);
	}

	dirs.push_back("/usr/share/pixmaps/");

	/* not in the Spec, but some systems puts KDE in 'opt' */
	dirs.push_back("/opt/kde/share/icons/");
}

static int check_size(int sz) {
	if(sz < ICON_SIZE_TINY || sz > ICON_SIZE_ENORMOUS) {
		/* got some unsupported size; return medium */
		return ICON_SIZE_MEDIUM;
	}

	return sz;
}

static IconContext figure_context(const String& ctx) {
	/* mandatory ones */
	if(ctx == "Actions")
		return ICON_CONTEXT_ACTION;
	if(ctx == "Devices")
		return ICON_CONTEXT_DEVICE;
	if(ctx == "FileSystems")
		return ICON_CONTEXT_FILESYSTEM;
	if(ctx == "MimeTypes")
		return ICON_CONTEXT_MIMETYPE;

	/* kde addons */
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

/*
 * If given theme inherits one, all theme_dirs should be prescanned for 
 * that theme; that applies too if inherited inherits another, and etc.
 */
void IconTheme::load_theme(const char* name) {
	bool   found = false;
	String index_path;
	Config c;

	/*
	 * Lookup throught the icon_directories + theme name; if found, try to locate index.theme since it is
	 * indicator for theme name
	 */
	for(StrListIter it = priv->theme_dirs.begin(); it != priv->theme_dirs.end(); ++it) {
		index_path = *it;
		index_path += name;
		index_path += E_DIR_SEPARATOR_STR "index.theme";

		if(c.load(index_path.c_str())) {
			found = true;
			break;
		}

		c.clear();
	}

	/* no 'index.theme' was found; we should quit */
	if(!found)
		return;

	char* tmp_buf = NULL;
	unsigned int tmp_buflen;

	if(!c.get_allocated("Icon Theme", "Directories", &tmp_buf, tmp_buflen)) {
		E_WARNING(E_STRLOC ": bad: %s\n", c.strerror());
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
	StrList dl;
	stringtok(dl, tmp_buf, ",");
	delete [] tmp_buf;

	/* used to load theme info and Context/Inherits */
	char static_buf[1024];

	/* load theme informations */
	if(!priv->info_loaded) {
		if(c.get_localized("Icon Theme", "Name", static_buf, sizeof(static_buf)))
			priv->curr_theme_stylized = static_buf;

		if(c.get_localized("Icon Theme", "Comment", static_buf, sizeof(static_buf)))
			priv->curr_theme_descr = static_buf;

		if(c.get("Icon Theme", "Example", static_buf, sizeof(static_buf)))
			priv->curr_theme_example = static_buf;

		priv->info_loaded = true;
	}

	int         size = 0;
	IconDirInfo dirinfo;
	IconContext context;
	String      theme_subdir_path;

	for(StrListIter it = dl.begin(); it != dl.end(); ++it) {
		/* remove spaces */
		(*it).trim();

		c.get((*it).c_str(), "Size", size, 0);
		size = check_size(size);

		if(c.get((*it).c_str(), "Context", static_buf, sizeof(static_buf)))
			context = figure_context(static_buf);
		else {
			/* 'Context' key not found */
			context = ICON_CONTEXT_ANY;
		}

		/* 
		 * Now, go through all theme directories and try to find subdirectory with 'Directories' values;
		 * if found, it will be considered as part of theme package with images and will be recorded.
		 *
		 * With this, when we have /usr/share/icons/foo/index.theme with '48x48/apps' but not
		 * /usr/share/icons/foo/48x48/apps, user can create e.g. ~/.icons/foo/48x48/apps and it will be considered
		 * as part of theme. At least pyxdg implementation does this too.
		 */
		for(StrListIter dit = priv->theme_dirs.begin(); dit != priv->theme_dirs.end(); ++dit) {
			theme_subdir_path = *dit;
			theme_subdir_path += name;
			theme_subdir_path += E_DIR_SEPARATOR_STR;
			theme_subdir_path += *it;

			if(dir_exists(theme_subdir_path.c_str())) {
				IconDirInfo dirinfo;
				dirinfo.path = theme_subdir_path;
				dirinfo.context = context;
				dirinfo.size = size;

				priv->dirlist.push_back(dirinfo);
			}
		}
	}
	
	/*
	 * Now try to read Inherits key which represent parents; if not found, default should be 'hicolor';
	 * this key can have multiple values, like:
	 *
	 * Inherits = theme1,theme2
	 *
	 * They all must be scanned :(
	 */
	if(c.get("Icon Theme", "Inherits", static_buf, sizeof(static_buf)))
		read_inherits(static_buf);
	else {
		/* prevent infinite recursion */
		if(!priv->fallback_visited) {
			priv->fallback_visited = true;
			load_theme(FALLBACK_THEME);
		}
	}
}

void IconTheme::read_inherits(const char* buf) {
	StrList parents;
	stringtok(parents, buf, ",");

	StrListIter it = parents.begin(), it_end = parents.end();

	for(; it != it_end; ++it) { 
		(*it).trim();
		load_theme((*it).c_str());
	}
}

void IconTheme::load(const char* name) {
	E_ASSERT(name != NULL);

	if(priv)
		clear();

	priv = new IconThemePrivate;
	priv->fallback_visited = false;
	priv->info_loaded = false;
	priv->curr_theme = name;

	init_base_dirs(priv->theme_dirs);
	load_theme(name);
}

void IconTheme::clear(void) {
	if(!priv)
		return;

	delete priv;
	priv = NULL;
}

String IconTheme::find_icon(const char* icon, IconSizes sz, IconContext ctx) {
	E_ASSERT(priv != NULL && "Did you call load() before this function?");

	if(priv->dirlist.empty())
		return "";

	String ret;
	ret.reserve(64);

	DirListIter it = priv->dirlist.begin(), it_end = priv->dirlist.end();

	/* ICON_CONTEXT_ANY means context is ignored, but also means slower lookup since all entries are searched */
	for(; it != it_end; ++it) {
		if((*it).size == sz && ((*it).context == ctx || ctx == ICON_CONTEXT_ANY)) {
			for(int i = 0; icon_extensions[i]; i++) {
				ret = (*it).path;
				ret += E_DIR_SEPARATOR_STR;
				ret += icon;
				ret += icon_extensions[i];

				/* only check if exists; not too good, but we do not handle file opennings */
				if(file_exists(ret.c_str()))
					return ret;
			}
		}
	}

	return "";
}

const char* IconTheme::theme_name(void) const {
	E_RETURN_VAL_IF_FAIL(priv != NULL, NULL);

	if(priv->curr_theme.empty())
		return NULL;
	return priv->curr_theme.c_str();
}

const char* IconTheme::stylized_theme_name(void) const {
	E_RETURN_VAL_IF_FAIL(priv != NULL, NULL);

	if(priv->curr_theme_stylized.empty())
		return NULL;
	return priv->curr_theme_stylized.c_str();
}

const char* IconTheme::description(void) const {
	E_RETURN_VAL_IF_FAIL(priv != NULL, NULL);

	if(priv->curr_theme_descr.empty())
		return NULL;
	return priv->curr_theme_descr.c_str();
}

const char* IconTheme::example_icon(void) const {
	E_RETURN_VAL_IF_FAIL(priv != NULL, NULL);

	if(priv->curr_theme_example.empty())
		return NULL;
	return priv->curr_theme_example.c_str();
}

void IconTheme::query_icons(list<String>& lst, IconSizes sz, IconContext ctx) const {
	E_RETURN_IF_FAIL(priv != NULL);

	if(priv->dirlist.size() == 0)
		return;

	StrList content;
	DirListIter it = priv->dirlist.begin(), it_end = priv->dirlist.end();

	for(; it != it_end; ++it) {
		if((*it).size == sz && ((*it).context == ctx || ctx == ICON_CONTEXT_ANY)) {
			if(dir_list((*it).path.c_str(), content, true))
				list_append(content, lst);
		}
	}
}

EDELIB_NS_END
