/*
 * $Id$
 *
 * Theming stuff
 * Copyright (c) 2009-2011 edelib authors
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

#include <string.h>
#include <stdlib.h>
#include <FL/Fl.H>
#include <FL/x.H>

#include <edelib/ThemeLoader.h>
#include <edelib/IconLoader.h>
#include <edelib/Color.h>
#include <edelib/ColorDb.h>
#include <edelib/Debug.h>
#include <edelib/Util.h>
#include <edelib/Resource.h>
#include <edelib/XSettingsClient.h>

EDELIB_NS_BEGIN

#define STORAGE_STR_SET(item, str) \
do {                               \
	if(item) free(item);           \
	item = strdup(str);            \
} while(0)

/* data that needs to be persistent */
struct StyleStorage {
	char *font_normal;
	char *font_bold;
	char *font_italic;

	StyleStorage() : font_normal(NULL), font_bold(NULL), font_italic(NULL) { }

	~StyleStorage() {
		if(font_normal) free(font_normal);
		if(font_bold)   free(font_bold);
		if(font_italic) free(font_italic);
	}
};

struct ThemeLoader_P {
	Theme           *curr_theme;
	XSettingsClient *xs;
	StyleStorage    *storage;
	ColorDb         *color_db;

	ThemeLoader_P() : curr_theme(NULL), xs(NULL), storage(NULL), color_db(NULL) { }

	~ThemeLoader_P() {
		delete curr_theme;
		delete xs;
		delete storage;
		delete color_db;
	}
};

static StyleStorage *theme_p_get_storage(ThemeLoader_P *t) {
	if(!t->storage)
		t->storage = new StyleStorage;
	return t->storage;
}

/* if color starts with '#', assume it it html color; otherwise it is from ColorDb database */
static bool figure_color(ColorDb *db,
						 const char *buf, 
						 unsigned char &r, 
						 unsigned char &g,
						 unsigned char &b)
{
	if(buf[0] == '#') {
		color_html_to_rgb(buf, r, g, b);
	} else {
		if(!db->find(buf, r, g, b)) {
			E_WARNING(E_STRLOC ": Unable to get color from X11 rgb database\n");
			return false;
		}
	}

	return true;
}

static void xsettings_cb(const char* name, 
						 XSettingsAction action,
						 XSettingsSetting* setting, 
						 void* data) 
{ }

ThemeLoader::ThemeLoader() : priv(NULL) {
	priv = new ThemeLoader_P;
}

ThemeLoader::~ThemeLoader() {
	delete priv;
}

void ThemeLoader::apply_common_gui_elements(void) {
	E_RETURN_IF_FAIL(priv);
	E_RETURN_IF_FAIL(priv->curr_theme);

	char          buf[128];
	long          font_sz, sh;
	unsigned char r, g, b;

	Theme *t = priv->curr_theme;

	/* load it only if wasn't loaded */
	if(!priv->color_db) {
		priv->color_db = new ColorDb;
		priv->color_db->load();
	}

	ColorDb *cb = priv->color_db;

	if(t->get_item("ede", "scheme", buf, sizeof(buf)))
		Fl::scheme(buf);

	if(t->get_item("ede", "background_color", buf, sizeof(buf))) {
		if(figure_color(cb, buf, r, g, b))
			Fl::background(r, g, b);
	}

	if(t->get_item("ede", "background_color2", buf, sizeof(buf))) {
		if(figure_color(cb, buf, r, g, b))
			Fl::background2(r, g, b);
	}

	if(t->get_item("ede", "foreground_color", buf, sizeof(buf))) {
		if(figure_color(cb, buf, r, g, b))
			Fl::foreground(r, g, b);
	}

	if(t->get_item("ede", "icon_theme", buf, sizeof(buf))) {
		if(IconLoader::inited())
			IconLoader::reload(buf);
	}

	if(t->get_item("ede", "scroll_width", sh, (long)Fl::scrollbar_size())) {
		Fl::scrollbar_size((int)sh);
	}

	/* TODO: document this: ignored if font_(normal|bold|italic) has given size */
	if(t->get_item("ede", "font_size", font_sz, 0)) {
		FL_NORMAL_SIZE = (int)font_sz;
		Fl::redraw();
	}

	if(t->get_item("ede", "font_normal", buf, sizeof(buf))) {
		StyleStorage *st = theme_p_get_storage(priv);
		STORAGE_STR_SET(st->font_normal, buf);

		Fl::set_font(FL_HELVETICA, st->font_normal);
	}

	if(t->get_item("ede", "font_bold", buf, sizeof(buf))) {
		StyleStorage *st = theme_p_get_storage(priv);
		STORAGE_STR_SET(st->font_bold, buf);

		Fl::set_font(FL_HELVETICA_BOLD, st->font_bold);
	}

	if(t->get_item("ede", "font_italic", buf, sizeof(buf))) {
		StyleStorage *st = theme_p_get_storage(priv);
		STORAGE_STR_SET(st->font_italic, buf);

		Fl::set_font(FL_HELVETICA_ITALIC, st->font_italic);
	}
}

bool ThemeLoader::load(const char *name, const char *prefix) {
	E_RETURN_VAL_IF_FAIL(priv, false);

	/* 'themes' is the main directory where we keep our themes. It is also compatible name with other desktops */
	String n = build_filename("themes", name);

	String path = Resource::find_data(n.c_str(), RES_SYS_ONLY, prefix);
	if(path.empty())
		return false;

	return load_with_path(path.c_str());
}

bool ThemeLoader::load_with_path(const char *path) {
	E_RETURN_VAL_IF_FAIL(priv, false);

	/* initialize Theme on demand */
	if(!priv->curr_theme)
		priv->curr_theme = new Theme;

	Theme *t = priv->curr_theme;

	/* unload current theme */
	if(t->loaded()) 
		t->clear();

	String p = build_filename(path, "main.ewt");
	bool ret = t->load(p.c_str());

	/* change the look if theme loaded successfully */
	if(ret) apply_common_gui_elements();
	return ret;
}

bool ThemeLoader::load_xsettings(void) {
	E_RETURN_VAL_IF_FAIL(priv, false);
	E_RETURN_VAL_IF_FAIL(fl_display, false);

	/* do not load it if already loaded */
	E_RETURN_VAL_IF_FAIL(priv->xs == NULL, true);

	XSettingsClient *x = new XSettingsClient();

	if(!x->init(fl_display, fl_screen, xsettings_cb)) {
		delete x;
		x = NULL;
	}

	priv->xs = x;
	return (priv->xs != NULL);
}

Theme *ThemeLoader::theme(void) {
	E_RETURN_VAL_IF_FAIL(priv, NULL);
	E_RETURN_VAL_IF_FAIL(priv->curr_theme, NULL);
	return priv->curr_theme;
}

XSettingsClient *ThemeLoader::xsettings(void) {
	E_RETURN_VAL_IF_FAIL(priv, NULL);
	E_RETURN_VAL_IF_FAIL(priv->xs, NULL);
	return priv->xs;
}

E_CLASS_GLOBAL_EXPLICIT_IMPLEMENT(ThemeLoader)

EDELIB_NS_END
