/*
 * $Id$
 *
 * Icon theme
 * Copyright (c) 2005-2014 edelib authors
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

#include <stdlib.h>
#include <edelib/IconTheme.h>
#include <edelib/Config.h>
#include <edelib/Util.h>
#include <edelib/StrUtil.h>
#include <edelib/Directory.h>
#include <edelib/FileTest.h>
#include <edelib/Missing.h>

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
	StrListIter it = from.begin(), ite = from.end();

	for(; it != ite; ++it)
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

	StrListIter it = xdg_data_dirs.begin(), ite = xdg_data_dirs.end();

	for(; it != ite; ++it) {
		path = *it;
		path += "/icons/";
		dirs.push_back(path);
	}

	dirs.push_back("/usr/share/pixmaps/");
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

	/* default */
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
	if(!found) return;

	char *saved, *dirs = NULL;
	unsigned int dirs_len;

	if(!c.get_allocated("Icon Theme", "Directories", &dirs, dirs_len)) {
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

	/* used to load theme info and Context/Inherits */
	char buf[1024];

	/* load theme informations */
	if(!priv->info_loaded) {
		if(c.get_localized("Icon Theme", "Name", buf, sizeof(buf)))
			priv->curr_theme_stylized = buf;

		if(c.get_localized("Icon Theme", "Comment", buf, sizeof(buf)))
			priv->curr_theme_descr = buf;

		if(c.get("Icon Theme", "Example", buf, sizeof(buf)))
			priv->curr_theme_example = buf;

		priv->info_loaded = true;
	}

	int         size;
	IconContext context;
	String      theme_subdir_path;

	for(char *dl = strtok_r(dirs, ",", &saved); dl; dl = strtok_r(NULL, ",", &saved)) {
		/* remove spaces */
		str_trim(dl);

		c.get(dl, "Size", size, 0);
		size = check_size(size);

		if(c.get(dl, "Context", buf, sizeof(buf)))
			context = figure_context(buf);
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
			theme_subdir_path += dl;

			if(file_test(theme_subdir_path.c_str(), FILE_TEST_IS_DIR)) {
				IconDirInfo inf;

				inf.path = theme_subdir_path;
				inf.context = context;
				inf.size = size;

				priv->dirlist.push_back(inf);
			}
		}
	}

	delete []dirs;
	
	/*
	 * Now try to read Inherits key which represent parents; if not found, default should be 'hicolor';
	 * this key can have multiple values, like:
	 *
	 * Inherits = theme1,theme2
	 *
	 * They all must be scanned :(
	 */
	if(c.get("Icon Theme", "Inherits", buf, sizeof(buf)))
		read_inherits(buf);
	else {
		/* prevent infinite recursion */
		if(!priv->fallback_visited) {
			priv->fallback_visited = true;
			load_theme(FALLBACK_THEME);
		}
	}
}

void IconTheme::read_inherits(const char* buf) {
	char *ptr, *parents = edelib_strndup(buf, 256);
	E_RETURN_IF_FAIL(parents != NULL);

	for(char *p = strtok_r(parents, ",", &ptr); p; p = strtok_r(NULL, ",", &ptr)) {
		str_trim(p);
		load_theme(p);
	}

	free(parents);
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
	if(!priv) return;

	delete priv;
	priv = NULL;
}

String IconTheme::find_icon(const char* icon, IconSizes sz, IconContext ctx) {
	E_ASSERT(priv != NULL && "Did you call load() before this function?");
	E_RETURN_VAL_IF_FAIL(priv->dirlist.size() > 0, "");

	String ret; ret.reserve(64);
	bool has_extension = false;

	/* handle the case when icon has extension */
	for(int i = 0; icon_extensions[i]; i++) {
		if(str_ends(icon, icon_extensions[i])) {
			has_extension = true;
			break;
		}
	}

	/* check if icon has extension; this is error, but check that anyway */
	DirListIter it = priv->dirlist.begin(), ite = priv->dirlist.end();

	/* ICON_CONTEXT_ANY means context is ignored, but also means slower lookup since all entries are searched */
	for(; it != ite; ++it) {
		if((*it).size == sz && ((*it).context == ctx || ctx == ICON_CONTEXT_ANY)) {
			if(has_extension) {
				ret = (*it).path;
				ret += E_DIR_SEPARATOR_STR;
				ret += icon;

				if(file_test(ret.c_str(), FILE_TEST_IS_REGULAR))
					return ret;
			} else {
				for(int i = 0; icon_extensions[i]; i++) {
					ret = (*it).path;
					ret += E_DIR_SEPARATOR_STR;
					ret += icon;
					ret += icon_extensions[i];

					/* only check if exists; not too good, but we do not handle file opennings */
					if(file_test(ret.c_str(), FILE_TEST_IS_REGULAR))
						return ret;
				}
			}
		}
	}

	/* the icon wasn't found in theme; search it in folders */
	StrListIter si = priv->theme_dirs.begin(), se = priv->theme_dirs.end();
	for(; si != se; ++si) {
		if(has_extension) {
			ret = *si;
			/* added dirs already have E_DIR_SEPARATOR_STR added */
			ret += icon;

			if(file_test(ret.c_str(), FILE_TEST_IS_REGULAR))
				return ret;
		} else {
			for(int i = 0; icon_extensions[i]; i++) {
				ret = *si;
				ret += icon;
				ret += icon_extensions[i];

				/* only check if exists; not too good, but we do not handle file opennings */
				if(file_test(ret.c_str(), FILE_TEST_IS_REGULAR))
					return ret;
			}
		}
	}

#ifndef ICON_THEME_FAST
	/* Third chance, search through icon theme, ignoring the size and context. Can be disabled to speed up things. */
	for(it = priv->dirlist.begin(); it != ite; ++it) {
		if(has_extension) {
			ret = (*it).path;
			ret += E_DIR_SEPARATOR_STR;
			ret += icon;

			if(file_test(ret.c_str(), FILE_TEST_IS_REGULAR))
				return ret;
		} else {
			for(int i = 0; icon_extensions[i]; i++) {
				ret = (*it).path;
				ret += E_DIR_SEPARATOR_STR;
				ret += icon;
				ret += icon_extensions[i];

				/* only check if exists; not too good, but we do not handle file opennings */
				if(file_test(ret.c_str(), FILE_TEST_IS_REGULAR))
					return ret;
			}
		}
	}
#endif

	/* nothing found */
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
	DirListIter it = priv->dirlist.begin(), ite = priv->dirlist.end();

	for(; it != ite; ++it) {
		if((*it).size == sz && ((*it).context == ctx || ctx == ICON_CONTEXT_ANY)) {
			if(dir_list((*it).path.c_str(), content, true))
				list_append(content, lst);
		}
	}
}

EDELIB_NS_END
