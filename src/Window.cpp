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
#include <edelib/Debug.h>

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <FL/Fl.h>
#include <FL/x.h>
#include <FL/Fl_Tooltip.h>
#include <FL/Fl_Shared_Image.h>

#ifdef HAVE_LIBXPM
	#include <X11/xpm.h>
#endif

#define EWINDOW 0xF3

#define DEFAULT_ICON_THEME "edeneu"
#define DEFAULT_FONT_SIZE  12

extern int FL_NORMAL_SIZE;

EDELIB_NS_BEGIN

XSettingsClient* xcli = NULL;
Window* local_window  = NULL;

char fl_show_iconic;	// hack for iconize()
int fl_disable_transient_for; // secret method of removing TRANSIENT_FOR

static const int childEventMask = ExposureMask;

static const int XEventMask = ExposureMask | StructureNotifyMask | 
	KeyPressMask | KeyReleaseMask | KeymapStateMask | FocusChangeMask | 
	ButtonPressMask | ButtonReleaseMask | 
	EnterWindowMask | LeaveWindowMask | 
	PointerMotionMask;

static void send_client_message(::Window w, Atom a, unsigned int uid) {
	XEvent xev;
	long mask;

	memset(&xev, 0, sizeof(xev));
	xev.xclient.type = ClientMessage;
	xev.xclient.window = w;
	xev.xclient.message_type = a;
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = uid;
	xev.xclient.data.l[1] = CurrentTime;
	mask = 0L;

	if(w == RootWindow(fl_display, fl_screen))
		mask = SubstructureRedirectMask;
	XSendEvent(fl_display, w, False, mask, &xev);
}

#if 0
static bool get_uint_property_value(::Window w, Atom at, unsigned int& val) {
	Atom type;
	int format;
	unsigned long n, extra;
	unsigned char* data = 0;

	int status = XGetWindowProperty(fl_display, w, at, 0L, 0x7fffffff, False, XA_CARDINAL, &type, &format, &n, &extra, 
			(unsigned char**)&data);

	if(status == Success && type != None) {
		val = int(*(long*)data);
		XFree(data);
		return true;
	}

	return false;
}

static bool get_uint_property_value_from_tree(::Window w, Atom at, ::Window& w_ret, unsigned int& val) {
	unsigned int nchildren = 0;
	bool status = false;
	::Window dummy, *children = 0;

	if(get_uint_property_value(w, at, val)) {
		w_ret = w;
		return true;
	}

	XQueryTree(fl_display, w, &dummy, &dummy, &children, &nchildren);
	if(!nchildren)
		return false;

	for(unsigned int i = 0; i < nchildren && !status; i++) {
		if(get_uint_property_value(children[i], at, val)) {
			w_ret = children[i];
			status = true;
			break;
		} else
			status = get_uint_property_value_from_tree(children[i], at, w_ret, val);
	}

	XFree(children);
	return status;
}

static bool set_uint_property_value(::Window w, Atom at, unsigned int val) {
	long prop = val;
	XChangeProperty(fl_display, w, at, XA_CARDINAL, 32, PropModeReplace, (unsigned char*)&prop, 1);
	return true;
}
#endif

/*
 * FIXME: make sure this function route events to window; e.g. ClientMessage
 * so window can install handler and re-read ClientMessage again
 */
static int xevent_handler(int id) {
#if 0
	if(local_window && local_window->settings_callback()) {
		if(fl_xevent->type == ClientMessage) {
			Atom message = fl_xevent->xclient.message_type;
			if(message == local_window->preferences_atom() && 
					(unsigned int)fl_xevent->xclient.data.l[0] == local_window->settings_uid()) {
				EDEBUG("==> %x\n", local_window->settings_callback_data());
				(*local_window->settings_callback())(local_window->settings_callback_data());
			}
		}
	}
#endif 

	if(fl_xevent->type == ClientMessage) {
		Fl_Window* win = 0;
		Atom message = fl_xevent->xclient.message_type;

		/*
		 * All windows known to FLTK must be scanned and global variable must NOT be used
		 * (sadly add_handler() don't have additional parameter that can be passed) due
		 * multiple Window objects in the same app. If global variable is used, callbacks
		 * will be messed up and only last Window object created will accept them.
		 *
		 * There are two ways to resolve this:
		 * - (used here) is to relay on FLTK's RTTI and scan window list for matched uid;
		 *   cons: every inherited window _must not_ decrease type() variable, only can increase it
		 *
		 * - relay on xclient.window member, but all X tree must be scanned for _CHANGE_SETTINGS property
		 *   and send it to those
		 *   cons: if there are large number of windows, this can be slow
		 *   pros: much more robust
		 */
		for(win = Fl::first_window(); win; win = Fl::next_window(win)) {
			if(win->type() >= EWINDOW) {
				edelib::Window* ewin = (edelib::Window*)win;
				if(message == ewin->preferences_atom() && 
						(unsigned int)fl_xevent->xclient.data.l[0] == ewin->settings_uid()) {
					(*ewin->settings_callback())(ewin->settings_callback_data());
				}
			}
		}
	}

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

		if(action == XSETTINGS_ACTION_DELETED || setting->type != XSETTINGS_TYPE_COLOR) {
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

		if(action == XSETTINGS_ACTION_DELETED || setting->type != XSETTINGS_TYPE_COLOR) {
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

		if(action == XSETTINGS_ACTION_DELETED || setting->type != XSETTINGS_TYPE_COLOR) {
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
		if(action == XSETTINGS_ACTION_DELETED || setting->type != XSETTINGS_TYPE_INT)
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
		if(action == XSETTINGS_ACTION_DELETED || setting->type != XSETTINGS_TYPE_STRING)
			icon_theme = DEFAULT_ICON_THEME;
		else
			icon_theme = setting->data.v_string;

		IconTheme::init(icon_theme);
		changed = true;
	} else if(win->xsettings_callback()) {
		// finally pass data to the user
		changed = (*win->xsettings_callback())(name, action, setting, win->xsettings_callback_data());
	}

	if(changed)
		win->redraw();
}

Window::Window(int X, int Y, int W, int H, const char* l) : Fl_Double_Window(X, Y, W, H, l), 
	inited(false), sbuffer(false), loaded_components(0), pref_atom(0), pref_uid(0), 
	xs_cb(NULL), xs_cb_old(NULL), xs_cb_data(NULL),
	s_cb(NULL), s_cb_data(NULL), icon_pixmap(NULL) { 

	type(EWINDOW);
}

Window::Window(int W, int H, const char* l) : Fl_Double_Window(W, H, l),
	inited(false), sbuffer(false), loaded_components(0), pref_atom(0), pref_uid(0), 
	xs_cb(NULL), xs_cb_old(NULL), xs_cb_data(NULL), 
	s_cb(NULL), s_cb_data(NULL), icon_pixmap(NULL) { 

	type(EWINDOW);
}

Window::~Window() {
	if(IconTheme::inited())
		IconTheme::shutdown();

	hide();
}

void Window::init(int component) {
	if(inited)
		return;

	if(component & WIN_INIT_IMAGES)
		fl_register_images();

	if(component & WIN_INIT_ICON_THEME)
		IconTheme::init(DEFAULT_ICON_THEME);

	// setup icons for dialogs
	themed_dialog_icons(MSGBOX_ICON_INFO, MSGBOX_ICON_ERROR, MSGBOX_ICON_QUESTION, 
			MSGBOX_ICON_QUESTION, MSGBOX_ICON_PASSWORD);

	fl_open_display();

	// FIXME: uh, uh ???
	local_window = this;

	pref_atom = XInternAtom(fl_display, "_CHANGE_SETTINGS", False);

	// FIXME: what if failed ?
	if(xs.init(fl_display, fl_screen, xsettings_cb, this))
		xcli = &xs;

	/*
	 * FLTK keeps internaly list of event handlers that are shared between
	 * windows in one application. If in one app exists two or more Window objects,
	 * handler will be added again without checking if already exists. With that 
	 * behaviour callback can be called multiple times even if only one window matches it.
	 *
	 * Removing (if exists) and re-adding again will assure we have only one callback
	 * per Window object.
	 */
	Fl::remove_handler(xevent_handler);
	Fl::add_handler(xevent_handler);

	inited = true;
}

/* static */
void Window::update_settings(unsigned int uid) {
	Atom pref = XInternAtom(fl_display, "_CHANGE_SETTINGS", True);
	// noone app set our atom
	if(pref == None)
		return;

#if 0
	::Window root = RootWindow(fl_display, fl_screen);
	::Window win_ret;
	unsigned int val = 0;

	if(get_uint_property_value_from_tree(root, pref, win_ret, val))
		send_client_message(win_ret, pref);
#endif

	/*
	 * Alternative version:
	 * instead recursively children scan, we will relay on server; let it
	 * pass message to the each child at the first level and send it uid. 
	 * If that uid matches child's one, callback will be done
	 */
	::Window dummy, root, *children = 0;
	unsigned int nchildren;

	root = RootWindow(fl_display, fl_screen);
	XQueryTree(fl_display, root, &dummy, &dummy, &children, &nchildren);
	if(!nchildren)
		return;

	for(unsigned int i = 0; i < nchildren; i++)
		if(children[i] != root)
			send_client_message(children[i], pref, uid);

	XFree(children);
}

XSettingsClient* Window::xsettings(void) {
	EASSERT(inited == true && "init() must be called before this function");
	return &xs;
}

static void set_titlebar_icon(Fl_Window* ww) {
#ifdef HAVE_LIBXPM
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

		if(type() == FL_WINDOW && ((box() == 1) || (box() & 2) && (box() <= 15))) {
			background_pixel = int(fl_xpixel(color()));
		}

		create_window_xid(this, set_titlebar_icon, background_pixel);
	} else
		XMapRaised(fl_display, fl_xid(this));

	if(pref_atom) {
		XChangeProperty(fl_display, fl_xid(this), 
				pref_atom, XA_CARDINAL, 32, PropModeReplace, (unsigned char*)&pref_uid, 1);
	}
}

// This function is stolen from FLTK and yes it is a mess !
void create_window_xid(Fl_Window* win, void (*before_map_func)(Fl_Window*), int background_pixel) {
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

		if(win->non_modal() && xp->next && !fl_disable_transient_for) {
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

		if(fl_show_iconic) {
			hints->flags |= StateHint;
			hints->initial_state = IconicState;
			fl_show_iconic = 0;
			showit = 0;
		}

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

EDELIB_NS_END
