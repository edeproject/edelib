/*
 * $Id$
 *
 * Window class
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>

#include <FL/x.H>
#include <FL/Fl_Tooltip.H>
#include <FL/Fl_Shared_Image.H>

#include <edelib/Window.h>
#include <edelib/Color.h>
#include <edelib/MessageBox.h>
#include <edelib/IconLoader.h>
#include <edelib/Debug.h>
#include <edelib/XSettingsClient.h>
#include <edelib/WindowXid.h>

#ifdef HAVE_LIBXPM
#include <X11/xpm.h>
#endif

#define DEFAULT_FONT_SIZE  12

extern int FL_NORMAL_SIZE;

EDELIB_NS_BEGIN

static char*            seen_theme = NULL;
static XSettingsClient* xcli = NULL;

static bool icon_theme_load_once(const char* theme) {
	if(seen_theme && strcmp(seen_theme, theme) == 0)
		return false;

	seen_theme = strdup(theme);
	E_DEBUG(E_STRLOC ": loading '%s' theme\n", theme);

	if(IconLoader::inited())
		IconLoader::reload(theme);
	else
		IconLoader::init(theme);
	return true;
}

static void set_titlebar_icon(Fl_Window* ww) {
#ifdef HAVE_LIBXPM
	/* it can be safely casted to our Window class */
	Window* win = (Window*)ww;

	if(!win->window_icon())
		return;

	Pixmap pix, mask;
	XpmCreatePixmapFromData(fl_display, DefaultRootWindow(fl_display), (char**)win->window_icon(), &pix, &mask, NULL);

	XWMHints* hints = XGetWMHints(fl_display, fl_xid(win));
	if(!hints)
		return;

	hints->icon_pixmap = pix;
	hints->icon_mask = mask;
	hints->flags |= IconPixmapHint | IconMaskHint;

	XSetWMHints(fl_display, fl_xid(win), hints);
	XFree(hints);
#endif
}

static int xevent_handler(int id) {
	if(xcli)
		xcli->process_xevent(fl_xevent);

	/* allow event could be processed again if user installs own handler */
	return 0;
}

static void xsettings_cb(const char* name, XSettingsAction action, XSettingsSetting* setting, void* data) {
	bool changed = false;
	Window* win = (Window*)data;

	if(strcmp(name, "Fltk/Background") == 0) {
		unsigned char r, g, b;

		if(action == XSETTINGS_ACTION_DELETED || setting->type != XSETTINGS_TYPE_COLOR) {
			/* FL_GRAY */
			r = g = b = 192;
		} else {
			r = setting->data.v_color.red;
			g = setting->data.v_color.green;
			b = setting->data.v_color.blue;
		}

		Fl::background(r, g, b);
		changed = true;
	} else if(strcmp(name, "Fltk/Background2") == 0) {
		unsigned char r, g, b;

		if(action == XSETTINGS_ACTION_DELETED || setting->type != XSETTINGS_TYPE_COLOR) {
			/* FL_WHITE */
			r = g = b = 255;
		} else {
			r = setting->data.v_color.red;
			g = setting->data.v_color.green;
			b = setting->data.v_color.blue;
		}

		Fl::background2(r, g, b);
		changed = true;
	} else if(strcmp(name, "Fltk/Foreground") == 0) {
		unsigned char r, g, b;

		if(action == XSETTINGS_ACTION_DELETED || setting->type != XSETTINGS_TYPE_COLOR) {
			/* FL_BLACK */
			r = g = b = 0;
		} else {
			r = setting->data.v_color.red;
			g = setting->data.v_color.green;
			b = setting->data.v_color.blue;
		}

		Fl::foreground(r, g, b);
		changed = true;
	} else if(strcmp(name, "Fltk/FontSize") == 0) {
		int normal_size;
		if(action == XSETTINGS_ACTION_DELETED || setting->type != XSETTINGS_TYPE_INT)
			normal_size = DEFAULT_FONT_SIZE;
		else
			normal_size = setting->data.v_int;
		/*
		 * XXX: A temporal hack until I figure something better.
		 *
		 * Ideal way would be to use got font size and call labelsize() on each child inside 
		 * window, but some of those childs can be Fl_Group which does not labelsize()
		 */
		FL_NORMAL_SIZE = normal_size;
		changed = true;
	} else if(strcmp(name, "Fltk/Scheme") == 0) {
		if(action != XSETTINGS_ACTION_DELETED && setting->type == XSETTINGS_TYPE_STRING && setting->data.v_string) {
				/* reloading is not needed because Fl::scheme() will do that for us */
				Fl::scheme(setting->data.v_string);
		}
	} else if(strcmp(name, "Net/IconThemeName") == 0 && win->component() & WIN_INIT_ICON_THEME) {
		const char* th = NULL;

		if(action == XSETTINGS_ACTION_DELETED || setting->type != XSETTINGS_TYPE_STRING)
			th = IconTheme::default_theme_name();
		else
			th = setting->data.v_string;

		changed = icon_theme_load_once(th);
	} else if(win->xsettings_callback()) {
		/* finally pass data to the user */
		changed = (*win->xsettings_callback())(name, action, setting, win->xsettings_callback_data());
	}

	if(changed)
		win->redraw();
}

Window::Window(int X, int Y, int W, int H, const char* l, int c) : Fl_Double_Window(X, Y, W, H, l), 
	sbuffer(false), loaded_components(0), 
	xs(NULL),
	xs_cb(NULL), xs_cb_old(NULL), xs_cb_data(NULL),
	icon_pixmap(NULL)
{
	init(c);
}

Window::Window(int W, int H, const char* l, int c) : Fl_Double_Window(W, H, l),
	sbuffer(false), loaded_components(0), 
	xs(NULL),
	xs_cb(NULL), xs_cb_old(NULL), xs_cb_data(NULL), 
	icon_pixmap(NULL)
{
	init(c);
}

Window::~Window() {
	IconLoader::shutdown();

	if(seen_theme)
		free(seen_theme);

	delete xs;
	hide();
}

void Window::init(int c) {
	if(c & WIN_INIT_IMAGES)
		fl_register_images();

	fl_open_display();

	if(c & WIN_INIT_ICON_THEME)
		icon_theme_load_once(IconTheme::default_theme_name());

	/* setup icons for dialogs */
	MessageBox::set_themed_icons(MSGBOX_ICON_INFO, 
								 MSGBOX_ICON_ERROR, 
								 MSGBOX_ICON_QUESTION, 
								 MSGBOX_ICON_QUESTION, 
								 MSGBOX_ICON_PASSWORD);

	/* XSETTINGS stuff */
	xs = new XSettingsClient();
	if(!xs->init(fl_display, fl_screen, xsettings_cb, this)) {
		delete xs;
		xs = NULL;
	}

	/* 
	 * add_handler() does not have parameter for optional data, so global storage must be used 
	 * XXX: does it collide with other Window objects?
	 */
	xcli = xs;

	/*
	 * FLTK keeps internaly list of event handlers that are shared between
	 * windows in one application. If in one app exists two or more Window objects,
	 * handler will be added again without checking if already exists. With that 
	 * behaviour callback can be called multiple times even if only one window matches it.
	 *
	 * Removing (if exists) and re-adding again will assure we have only one callback per Window object.
	 */
	Fl::remove_handler(xevent_handler);
	Fl::add_handler(xevent_handler);
}

void Window::show(void) {
	image(Fl::scheme_bg_);

	if(Fl::scheme_bg_) {
		labeltype(FL_NORMAL_LABEL);
		align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
	} else {
		labeltype(FL_NO_LABEL);
	}

	Fl_Tooltip::exit(this);

	if(!shown()) {
		// Don't set background pixel for double-buffered windows...
		int background_pixel = -1;

		if(type() == FL_WINDOW && ((box() == 1) || ((box() & 2) && (box() <= 15)))) {
			background_pixel = int(fl_xpixel(color()));
		}

		window_xid_create(this, set_titlebar_icon, background_pixel);
	} else {
		XMapRaised(fl_display, fl_xid(this));
	}
}

EDELIB_NS_END
