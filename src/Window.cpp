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
#include <string.h>
#include <stdio.h>
#include <FL/Fl.h>
#include <FL/x.h>

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
	if(strcmp(name, "Fltk/Background") == 0) {
		unsigned char r, g, b;

		if(action == XSETTINGS_ACTION_DELETED) {
			// FL_GRAY == #c0c0c0
			color_fltk_to_rgb(FL_GRAY, r, g, b);
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
			unsigned char r, g, b;
			// FL_WHITE == #ffffff
			color_fltk_to_rgb(FL_WHITE, r, g, b);
		} else {
			r = setting->data.v_color.red;
			g = setting->data.v_color.green;
			b = setting->data.v_color.blue;
		}

		Fl::background2(r, g, b);
		changed = true;
	}

	if(changed) {
		Window* win = (Window*)data;
		win->redraw();
	}
}

Window::Window(int X, int Y, int W, int H, const char* l) : Fl_Window(X, Y, W, H, l) { 
	init();
}

Window::Window(int W, int H, const char* l) : Fl_Window(W, H, l) { 
	init();
}

Window::~Window() {
}

void Window::init(void) {
}

void Window::show(void) {
	Fl_Window::show();

	if(xs.init(fl_display, fl_screen, xsettings_cb, this)) {
		xcli = &xs;
		Fl::add_handler(xevent_handler);
	}
}

EDELIB_NS_END
