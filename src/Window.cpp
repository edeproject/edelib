/*
 * $Id$
 *
 * Window class
 * Part of edelib.
 * Copyright (c) 2005-2007 EDE Authors.
 *
 * This program is licensed under terms of the 
 * GNU General Public License version 2 or newer.
 * See COPYING for details.
 */

#include <edelib/Window.h>
#include <edelib/Color.h>
#include <edelib/MessageBox.h>
#include <edelib/IconTheme.h>
#include <string.h>
#include <stdio.h>
#include <FL/Fl.h>
#include <FL/x.h>

extern int FL_NORMAL_SIZE;

#define DEFAULT_ICON_THEME "edeneu"
#define DEFAULT_FONT_SIZE  12

EDELIB_NS_BEGIN

XSettingsClient* xcli = NULL;

static int xevent_handler(int id) {
	if(xcli) 
		return xcli->process_xevent(fl_xevent);
	else
		return 0;
}

static void xsettings_cb(const char* name, XSettingsAction action, XSettingsSetting* setting, void* data) {
	bool changed = false;
	Window* win = (Window*)data;

	if(strcmp(name, "Fltk/Background") == 0) {
		unsigned char r, g, b;

		if(action == XSETTINGS_ACTION_DELETED) {
			// FL_GRAY
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

		if(action == XSETTINGS_ACTION_DELETED) {
			// FL_WHITE
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

		if(action == XSETTINGS_ACTION_DELETED) {
			// FL_BLACK
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
		if(action == XSETTINGS_ACTION_DELETED)
			normal_size = DEFAULT_FONT_SIZE;
		else
			normal_size = setting->data.v_int;
		/*
		 * A temporal hack until I figure something better.
		 *
		 * Ideal way would be to use got font size and call labelsize() 
		 * on each child inside window, but some of those childs can be 
		 * Fl_Group which does not labelsize()
		 */
		FL_NORMAL_SIZE = normal_size;
		changed = true;
	} else if(strcmp(name, "Net/IconThemeName") == 0) {
		if(IconTheme::inited())
			IconTheme::shutdown();

		const char* icon_theme;
		if(action == XSETTINGS_ACTION_DELETED)
			icon_theme = DEFAULT_ICON_THEME;
		else
			icon_theme = setting->data.v_string;

		IconTheme::init(icon_theme);
		changed = true;
	}

	if(changed)
		win->redraw();
}

Window::Window(int X, int Y, int W, int H, const char* l) : Fl_Window(X, Y, W, H, l), inited(false) { 
	init();
}

Window::Window(int W, int H, const char* l) : Fl_Window(W, H, l), inited(false) { 
	init();
}

Window::~Window() {
	if(IconTheme::inited())
		IconTheme::shutdown();
}

void Window::init(void) {
	if(inited)
		return;

	fl_open_display();

	IconTheme::init(DEFAULT_ICON_THEME);

	// setup icons for dialogs
	themed_dialog_icons(MSGBOX_ICON_INFO, MSGBOX_ICON_ERROR, MSGBOX_ICON_QUESTION, 
			MSGBOX_ICON_QUESTION, MSGBOX_ICON_PASSWORD);

	if(xs.init(fl_display, fl_screen, xsettings_cb, this)) {
		xcli = &xs;
		Fl::add_handler(xevent_handler);
	}

	inited = true;
}

void Window::show(void) {
	Fl_Window::show();
	init();
}

EDELIB_NS_END
