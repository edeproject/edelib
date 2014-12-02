/*
 * $Id$
 *
 * Window utils
 * Copyright (c) 1998-2006 by Bill Spitzak and others
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

#include <edelib/WindowUtils.h>

#include <ctype.h>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>

/* 1.3.0 requires this */
#define FL_LIBRARY   1
#include <FL/x.H>

/* 
 * These are defined in FLTK as hidden variables for some internal hacks, but are used here.
 * XXX: possible changes in future FLTK versions
 */
#if 0
extern char fl_show_iconic;
extern int  fl_disable_transient_for;
#endif 

EDELIB_NS_BEGIN

static const int childEventMask = ExposureMask;

static const int XEventMask = ExposureMask | StructureNotifyMask | 
	KeyPressMask | KeyReleaseMask | KeymapStateMask | FocusChangeMask | 
	ButtonPressMask | ButtonReleaseMask | 
	EnterWindowMask | LeaveWindowMask | 
	PointerMotionMask;

/* stolen from FLTK */
void window_xid_create(Fl_Window* win, void (*before_map_func)(Fl_Window*), int background_pixel) {
	Fl_Group::current(0); // get rid of very common user bug: forgot end()

	int X = win->x();
	int Y = win->y();
	int W = win->w();
	if(W <= 0) W = 1; // X don't like zero...
	int H = win->h();
	if(H <= 0) H = 1; // X don't like zero...
	if(!win->parent() && !Fl::grab()) {
		// center windows in case window manager does not do anything:
#ifdef FL_CENTER_WINDOWS
		if(!(win->flags() & Fl_Window::FL_FORCE_POSITION)) {
			win->x(X = scr_x+(scr_w-W)/2);
			win->y(Y = scr_y+(scr_h-H)/2);
		}
#endif // FL_CENTER_WINDOWS

		// force the window to be on-screen.  Usually the X window manager
		// does this, but a few don't, so we do it here for consistency:
		int scr_x, scr_y, scr_w, scr_h;
		Fl::screen_xywh(scr_x, scr_y, scr_w, scr_h, X, Y);

		if(win->border()) {
			// ensure border is on screen:
			// (assumme extremely minimal dimensions for this border)
			const int top = 20;
			const int left = 1;
			const int right = 1;
			const int bottom = 1;
			if(X+W+right > scr_x+scr_w) X = scr_x+scr_w-right-W;
			if(X-left < scr_x) X = scr_x+left;
			if(Y+H+bottom > scr_y+scr_h) Y = scr_y+scr_h-bottom-H;
			if(Y-top < scr_y) Y = scr_y+top;
		}

		// now insure contents are on-screen (more important than border):
		if(X+W > scr_x+scr_w) X = scr_x+scr_w-W;
		if(X < scr_x) X = scr_x;
		if(Y+H > scr_y+scr_h) Y = scr_y+scr_h-H;
		if(Y < scr_y) Y = scr_y;
	}

	unsigned long root = win->parent() ?  fl_xid(win->window()) : RootWindow(fl_display, fl_screen);

	XSetWindowAttributes attr;
	int mask = CWBorderPixel|CWColormap|CWEventMask|CWBitGravity;
	attr.event_mask = win->parent() ? childEventMask : XEventMask;
	attr.colormap = fl_colormap; // use default FLTK colormap
	attr.border_pixel = 0;
	attr.bit_gravity = 0; // StaticGravity;
	if(win->override()) {
		attr.override_redirect = 1;
		attr.save_under = 1;
		mask |= CWOverrideRedirect | CWSaveUnder;
	} else
		attr.override_redirect = 0;

	if(Fl::grab()) {
		attr.save_under = 1; 
		mask |= CWSaveUnder;

		if(!win->border()) {
			attr.override_redirect = 1; 
			mask |= CWOverrideRedirect;
		}
	}

	if(background_pixel >= 0) {
		attr.background_pixel = background_pixel;
		background_pixel = -1;
		mask |= CWBackPixel;
	}

	Fl_X* xp = Fl_X::set_xid(win, XCreateWindow(fl_display, root, 
				X, Y, W, H,
				0, // borderwidth
				fl_visual->depth, // use default FLTK visual
				InputOutput,
				fl_visual->visual, // use default FLTK visual
				mask, &attr));

	int showit = 1;

	if(!win->parent() && !attr.override_redirect) {
		// Communicate all kinds 'o junk to the X Window Manager:
		win->label(win->label(), win->iconlabel());

		Atom WM_PROTOCOLS = XInternAtom(fl_display, "WM_PROTOCOLS", 0);
		Atom WM_DELETE_WINDOW = XInternAtom(fl_display, "WM_DELETE_WINDOW", 0);

		XChangeProperty(fl_display, xp->xid, WM_PROTOCOLS, XA_ATOM, 32, 0, (unsigned char*)&WM_DELETE_WINDOW, 1);

		// send size limits and border:
		xp->sendxjunk();

		// set the class property, which controls the icon used:
		if(win->xclass()) {
			char buffer[1024];
			char *p; 
			const char *q;
			// truncate on any punctuation, because they break XResource lookup:
			for(p = buffer, q = win->xclass(); isalnum(*q)||(*q&128); )
				*p++ = *q++;
			*p++ = 0;
			// create the capitalized version:
			q = buffer;
			*p = toupper(*q++); 

			if(*p++ == 'X') 
				*p++ = toupper(*q++);

			while((*p++ = *q++))
				;

			XChangeProperty(fl_display, xp->xid, XA_WM_CLASS, XA_STRING, 8, 0, (unsigned char *)buffer, p-buffer-1);
		}

		if(win->non_modal() && xp->next /* && !fl_disable_transient_for */) {
			// find some other window to be "transient for":
			Fl_Window* wp = xp->next->w;
			while(wp->parent()) 
				wp = wp->window();

			XSetTransientForHint(fl_display, xp->xid, fl_xid(wp));
			if(!wp->visible())
				showit = 0; // guess that wm will not show it
		}
	   
		// Make sure that borderless windows do not show in the task bar
		if(!win->border()) {
			Atom net_wm_state = XInternAtom (fl_display, "_NET_WM_STATE", 0);
			Atom net_wm_state_skip_taskbar = XInternAtom (fl_display, "_NET_WM_STATE_SKIP_TASKBAR", 0);

			XChangeProperty (fl_display, xp->xid, net_wm_state, XA_ATOM, 32, 
					PropModeAppend, (unsigned char*) &net_wm_state_skip_taskbar, 1);
		}

		// Make it receptive to DnD:
		long version = 4;
		Atom xdndaware = XInternAtom(fl_display, "XdndAware", 0);
		XChangeProperty(fl_display, xp->xid, xdndaware, XA_ATOM, sizeof(int)*8, 0, (unsigned char*)&version, 1);

		XWMHints *hints = XAllocWMHints();
		hints->input = True;
		hints->flags = InputHint;
#if 0 
		if(fl_show_iconic) {
			hints->flags |= StateHint;
			hints->initial_state = IconicState;
			fl_show_iconic = 0;
			showit = 0;
		}
#endif 

		// This is not removed so it can be used with windows inherited from Fl_Window
		if(win->icon()) {
			hints->icon_pixmap = (Pixmap)win->icon();
			hints->flags |= IconPixmapHint;
		}

		XSetWMHints(fl_display, xp->xid, hints);
		XFree(hints);
	}

	if(before_map_func)
		(*before_map_func)(win);

	XMapWindow(fl_display, xp->xid);

	if(showit) {
		win->set_visible();
		int old_event = Fl::e_number;
		win->handle(Fl::e_number = FL_SHOW); // get child windows to appear
		Fl::e_number = old_event;
		win->redraw();
	}
}

void window_center_on_screen(Fl_Window *win) {
	int X, Y, W, H;
	Fl::screen_xywh(X, Y, W, H);
	win->position(W/2 - win->w()/2, H/2 - win->h()/2);
}

EDELIB_NS_END
