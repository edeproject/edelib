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
#include <edelib/FontCache.h>

#ifdef EDELIB_HAVE_RTTI
# include <FL/Fl_Browser_.H>
# include <FL/Fl_Input_.H>
# include <FL/Fl_Menu_.H>
# include <FL/Fl_Text_Display.H>
#endif

extern int FL_NORMAL_SIZE;

EDELIB_NS_BEGIN

#define STORAGE_STR_SET(item, str) \
do {                               \
	if(item) free(item);           \
	item = strdup(str);            \
} while(0)

struct ThemeLoader_P {
	Theme           *curr_theme;
	XSettingsClient *xs;
	ColorDb         *color_db;

	ThemeLoader_P() : curr_theme(NULL), xs(NULL),  color_db(NULL) { }

	~ThemeLoader_P() {
		delete curr_theme;
		delete xs;
		delete color_db;
	}
};

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

#ifdef EDELIB_HAVE_RTTI
#define TRY_UPDATE_TEXTSIZE(klass, o, sz)		\
do {											\
	klass *obj = dynamic_cast<klass*>(o);		\
	if(obj) {									\
		obj->textsize(sz);						\
		continue;								\
	}											\
} while(0)

static void update_fonts_on_group(Fl_Group *g, int sz) {
	int n = g->children();
	if(!n) return;

	Fl_Widget *o;

	for(int i = 0; i < n; i++) {
		o = g->child(i);
		if(dynamic_cast<Fl_Group*>(o))
			update_fonts_on_group((Fl_Group*)o, sz);

		o->labelsize(sz);

		TRY_UPDATE_TEXTSIZE(Fl_Browser_, o, sz);
		TRY_UPDATE_TEXTSIZE(Fl_Input_, o, sz);
		TRY_UPDATE_TEXTSIZE(Fl_Menu_, o, sz);
		TRY_UPDATE_TEXTSIZE(Fl_Text_Display, o, sz);
	}
}
#endif /* EDELIB_HAVE_RTTI */

/*
 * update fonts by scanning widgets top-down; this could be a slow as we
 * must use RTTI here, since FLTK provides incomplete RTTI support :S
 */
static void update_fonts(int sz) {
#ifdef EDELIB_HAVE_RTTI
	for(Fl_Window *i = Fl::first_window(); i; i = Fl::next_window(i))
		update_fonts_on_group(i, sz);
#else
	Fl::redraw();
#endif
}

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

	if(t->get_item("ede", "selection_color", buf, sizeof(buf))) {
		if(figure_color(cb, buf, r, g, b))
			Fl::set_color(FL_SELECTION_COLOR, r, g, b);
	}

	if(t->get_item("ede", "icon_theme", buf, sizeof(buf))) {
		if(IconLoader::inited())
			IconLoader::reload(buf);
	}

	/* global scrollbar size is until 1.1.10 */
#if (FL_MAJOR_VERSION >= 1) && ((FL_MINOR_VERSION >= 3) || ((FL_MINOR_VERSION >= 1) && (FL_PATCH_VERSION >= 10)))
	long sh;
	if(t->get_item("ede", "scrollbar_width", sh, (long)Fl::scrollbar_size())) {
		Fl::scrollbar_size((int)sh);
	}
#endif

	if(t->get_item("ede", "font", buf, sizeof(buf))) {
		Fl_Font f;
		int     fs;

		font_cache_find(buf, f, fs, FL_HELVETICA, 12);

		Fl::set_font(FL_HELVETICA, f);
		/* for future shown windows */
		FL_NORMAL_SIZE = fs;
		update_fonts(FL_NORMAL_SIZE);
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
