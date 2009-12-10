/*
 * $Id$
 *
 * Functions for easier communication with window manager
 * Copyright (c) 2009 edelib authors
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
#include <X11/Xproto.h>
#include <FL/Fl.H>
#include <FL/x.H>

#include <edelib/Debug.h>
#include <edelib/List.h>
#include <edelib/Netwm.h>

EDELIB_NS_BEGIN

struct NetwmCallbackData {
	NetwmCallback  cb;
	void		   *data;
};

typedef list<NetwmCallbackData> CbList;
typedef list<NetwmCallbackData>::iterator CbListIt;

static CbList callback_list;
static bool   input_selected = false;

static Atom _XA_NET_WORKAREA;

static Atom _XA_NET_WM_WINDOW_TYPE;
static Atom _XA_NET_WM_WINDOW_TYPE_DOCK;
static Atom _XA_NET_WM_WINDOW_TYPE_DESKTOP;
static Atom _XA_NET_WM_WINDOW_TYPE_SPLASH;
static Atom _XA_NET_WM_WINDOW_TYPE_NORMAL;
static Atom _XA_NET_WM_WINDOW_TYPE_TOOLBAR;
static Atom _XA_NET_WM_WINDOW_TYPE_MENU;
static Atom _XA_NET_WM_WINDOW_TYPE_UTILITY;
static Atom _XA_NET_WM_WINDOW_TYPE_DIALOG;
static Atom _XA_NET_WM_WINDOW_TYPE_DROPDOWN_MENU;
static Atom _XA_NET_WM_WINDOW_TYPE_POPUP_MENU;
static Atom _XA_NET_WM_WINDOW_TYPE_TOOLTIP;
static Atom _XA_NET_WM_WINDOW_TYPE_NOTIFICATION;
static Atom _XA_NET_WM_WINDOW_TYPE_COMBO;
static Atom _XA_NET_WM_WINDOW_TYPE_DND;

static Atom _XA_NET_WM_STRUT;
static Atom _XA_NET_NUMBER_OF_DESKTOPS;
static Atom _XA_NET_CURRENT_DESKTOP;
static Atom _XA_NET_DESKTOP_NAMES;
static Atom _XA_NET_CLIENT_LIST;
static Atom _XA_NET_WM_DESKTOP;
static Atom _XA_NET_WM_NAME;
static Atom _XA_NET_WM_VISIBLE_NAME;
static Atom _XA_NET_ACTIVE_WINDOW;
static Atom _XA_NET_CLOSE_WINDOW;

static Atom _XA_WM_STATE;
static Atom _XA_NET_WM_STATE;
static Atom _XA_NET_WM_STATE_MAXIMIZED_HORZ;
static Atom _XA_NET_WM_STATE_MAXIMIZED_VERT;

static Atom _XA_EDE_WM_ACTION;
static Atom _XA_EDE_WM_RESTORE_SIZE;

/* this macro is set in xlib when X-es provides UTF-8 extension (since XFree86 4.0.2) */
#if X_HAVE_UTF8_STRING
static Atom _XA_UTF8_STRING;
#else
# define _XA_UTF8_STRING XA_STRING
#endif

#define REGISTER_ATOM(var, str) var = XInternAtom(fl_display, str, False)

static short atoms_inited = 0;

static void init_atoms_once(void) {
	if(atoms_inited)
		return;

	REGISTER_ATOM(_XA_NET_WORKAREA,                     "_NET_WORKAREA");

	REGISTER_ATOM(_XA_NET_WM_WINDOW_TYPE,               "_NET_WM_WINDOW_TYPE");
	REGISTER_ATOM(_XA_NET_WM_WINDOW_TYPE_NORMAL,        "_NET_WM_WINDOW_TYPE_NORMAL");
	REGISTER_ATOM(_XA_NET_WM_WINDOW_TYPE_DOCK,          "_NET_WM_WINDOW_TYPE_DOCK");
	REGISTER_ATOM(_XA_NET_WM_WINDOW_TYPE_SPLASH,        "_NET_WM_WINDOW_TYPE_SPLASH");
	REGISTER_ATOM(_XA_NET_WM_WINDOW_TYPE_DESKTOP,       "_NET_WM_WINDOW_TYPE_DESKTOP");
	REGISTER_ATOM(_XA_NET_WM_WINDOW_TYPE_TOOLBAR,       "_NET_WM_WINDOW_TYPE_TOOLBAR");
	REGISTER_ATOM(_XA_NET_WM_WINDOW_TYPE_MENU,          "_NET_WM_WINDOW_TYPE_MENU");
	REGISTER_ATOM(_XA_NET_WM_WINDOW_TYPE_UTILITY,       "_NET_WM_WINDOW_TYPE_UTILITY");

	REGISTER_ATOM(_XA_NET_WM_WINDOW_TYPE_DIALOG,        "_NET_WM_WINDOW_TYPE_DIALOG");
	REGISTER_ATOM(_XA_NET_WM_WINDOW_TYPE_DROPDOWN_MENU, "_NET_WM_WINDOW_TYPE_DROPDOWN_MENU");
	REGISTER_ATOM(_XA_NET_WM_WINDOW_TYPE_POPUP_MENU,    "_NET_WM_WINDOW_TYPE_POPUP_MENU");
	REGISTER_ATOM(_XA_NET_WM_WINDOW_TYPE_TOOLTIP,       "_NET_WM_WINDOW_TYPE_TOOLTIP");
	REGISTER_ATOM(_XA_NET_WM_WINDOW_TYPE_NOTIFICATION,  "_NET_WM_WINDOW_TYPE_NOTIFICATION");
	REGISTER_ATOM(_XA_NET_WM_WINDOW_TYPE_COMBO,         "_NET_WM_WINDOW_TYPE_COMBO");
	REGISTER_ATOM(_XA_NET_WM_WINDOW_TYPE_DND,           "_NET_WM_WINDOW_TYPE_DND");

	REGISTER_ATOM(_XA_NET_WM_STRUT,                     "_NET_WM_STRUT");
	REGISTER_ATOM(_XA_NET_NUMBER_OF_DESKTOPS,           "_NET_NUMBER_OF_DESKTOPS");
	REGISTER_ATOM(_XA_NET_CURRENT_DESKTOP,              "_NET_CURRENT_DESKTOP");
	REGISTER_ATOM(_XA_NET_DESKTOP_NAMES,                "_NET_DESKTOP_NAMES");
	REGISTER_ATOM(_XA_NET_CLIENT_LIST,                  "_NET_CLIENT_LIST");
	REGISTER_ATOM(_XA_NET_WM_DESKTOP,                   "_NET_WM_DESKTOP");
	REGISTER_ATOM(_XA_NET_WM_NAME,                      "_NET_WM_NAME");
	REGISTER_ATOM(_XA_NET_WM_VISIBLE_NAME,              "_NET_WM_VISIBLE_NAME");
	REGISTER_ATOM(_XA_NET_ACTIVE_WINDOW,                "_NET_ACTIVE_WINDOW");
	REGISTER_ATOM(_XA_NET_CLOSE_WINDOW,                 "_NET_CLOSE_WINDOW");

	/* this one should be in some X header */
	REGISTER_ATOM(_XA_WM_STATE,                         "WM_STATE");

	REGISTER_ATOM(_XA_NET_WM_STATE,                     "_NET_WM_STATE");
	REGISTER_ATOM(_XA_NET_WM_STATE_MAXIMIZED_HORZ,      "_NET_WM_STATE_MAXIMIZED_HORZ");
	REGISTER_ATOM(_XA_NET_WM_STATE_MAXIMIZED_VERT,      "_NET_WM_STATE_MAXIMIZED_VERT");

	/* edewm specific */
	REGISTER_ATOM(_XA_EDE_WM_ACTION,                    "_EDE_WM_ACTION");
	REGISTER_ATOM(_XA_EDE_WM_RESTORE_SIZE,              "_EDE_WM_RESTORE_SIZE");

#ifdef X_HAVE_UTF8_STRING
	REGISTER_ATOM(_XA_UTF8_STRING, "UTF8_STRING");
#endif
	atoms_inited = 1;
}

static int xevent_handler(int e) {
	if(fl_xevent->type == PropertyNotify) {
		int action = -1;

		if(fl_xevent->xproperty.atom == _XA_NET_NUMBER_OF_DESKTOPS)
			action = NETWM_CHANGED_WORKSPACE_COUNT;
		else if(fl_xevent->xproperty.atom == _XA_NET_DESKTOP_NAMES)
			action = NETWM_CHANGED_WORKSPACE_NAMES;
		else if(fl_xevent->xproperty.atom == _XA_NET_CURRENT_DESKTOP)
			action = NETWM_CHANGED_CURRENT_WORKSPACE;
		else if(fl_xevent->xproperty.atom == _XA_NET_WORKAREA)
			action = NETWM_CHANGED_CURRENT_WORKAREA;
		else if(fl_xevent->xproperty.atom == _XA_NET_ACTIVE_WINDOW)
			action = NETWM_CHANGED_ACTIVE_WINDOW;
		else if(fl_xevent->xproperty.atom == _XA_NET_WM_NAME || fl_xevent->xproperty.atom == XA_WM_NAME)
			action = NETWM_CHANGED_WINDOW_NAME;
		else if(fl_xevent->xproperty.atom == _XA_NET_WM_VISIBLE_NAME)
			action = NETWM_CHANGED_WINDOW_VISIBLE_NAME;
		else if(fl_xevent->xproperty.atom == _XA_NET_WM_DESKTOP)
			action = NETWM_CHANGED_WINDOW_DESKTOP;
		else if(fl_xevent->xproperty.atom == _XA_NET_CLIENT_LIST)
			action = NETWM_CHANGED_WINDOW_LIST;
		else
			action = -1;

		if(action >= 0 && !callback_list.empty()) {
			Window xid = fl_xevent->xproperty.window;

			/* TODO: locking here */
			CbListIt it = callback_list.begin(), it_end = callback_list.end();
			for(; it != it_end; ++it) {
				/* execute callback */
				(*it).cb(action, xid, (*it).data);
			}
		}
	}

	return 0;
}

void netwm_callback_add(NetwmCallback cb, void *data) {
	E_RETURN_IF_FAIL(cb != NULL);

	fl_open_display();
	init_atoms_once();

	/* to catch _XA_NET_CURRENT_DESKTOP and such events */
	if(!input_selected) {
		XSelectInput(fl_display, RootWindow(fl_display, fl_screen), PropertyChangeMask | StructureNotifyMask);
		input_selected = true;
	}

	NetwmCallbackData cb_data;
	cb_data.cb = cb;
	cb_data.data = data;

	callback_list.push_back(cb_data);

	Fl::remove_handler(xevent_handler);
	Fl::add_handler(xevent_handler);
}

void netwm_callback_remove(NetwmCallback cb) {
	if(callback_list.empty())
		return;

	CbListIt it = callback_list.begin(), it_end = callback_list.end();
	while(it != it_end) {
		if(cb == (*it).cb) {
			it = callback_list.erase(it);
		} else {
			/* continue, in case the same callback is put multiple times */
			++it;
		}
	}
}

bool netwm_workarea_get_size(int& x, int& y, int& w, int &h) {
	init_atoms_once();

	Atom real;
	int  format;
	unsigned long n, extra;
	unsigned char* prop;
	x = y = w = h = 0;

	int status = XGetWindowProperty(fl_display, RootWindow(fl_display, fl_screen), 
			_XA_NET_WORKAREA, 0L, 0x7fffffff, False, XA_CARDINAL, &real, &format, &n, &extra, (unsigned char**)&prop);

	if(status != Success)
		return false;

	CARD32* val = (CARD32*)prop;
	if(val) {
		x = val[0];
		y = val[1];
		w = val[2];
		h = val[3];

		XFree((char*)val);
		return true;
	}

	return false;
}

int netwm_workspace_get_count(void) {
	init_atoms_once();

	Atom real;
	int format;
	unsigned long n, extra;
	unsigned char* prop;

	int status = XGetWindowProperty(fl_display, RootWindow(fl_display, fl_screen), 
			_XA_NET_NUMBER_OF_DESKTOPS, 0L, 0x7fffffff, False, XA_CARDINAL, &real, &format, &n, &extra, 
			(unsigned char**)&prop);

	if(status != Success || !prop)
		return -1;

	int ns = int(*(long*)prop);
	XFree(prop);
	return ns;
}

void netwm_workspace_change(int n) {
	init_atoms_once();

	XEvent xev;
	Window root_win = RootWindow(fl_display, fl_screen);

	memset(&xev, 0, sizeof(xev));
	xev.xclient.type = ClientMessage;
	xev.xclient.serial = 0;
	xev.xclient.send_event = True;
	xev.xclient.window = root_win;
	xev.xclient.display = fl_display;
	xev.xclient.message_type = _XA_NET_CURRENT_DESKTOP;
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = (long)n;
	xev.xclient.data.l[1] = CurrentTime;

	XSendEvent (fl_display, root_win, False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);
	XSync(fl_display, True);
}

int netwm_workspace_get_current(void) {
	init_atoms_once();

	Atom real;
	int format;
	unsigned long n, extra;
	unsigned char* prop;

	int status = XGetWindowProperty(fl_display, RootWindow(fl_display, fl_screen), 
			_XA_NET_CURRENT_DESKTOP, 0L, 0x7fffffff, False, XA_CARDINAL, &real, &format, &n, &extra, 
			(unsigned char**)&prop);

	if(status != Success || !prop)
		return -1;

	int ns = int(*(long*)prop);
	XFree(prop);
	return ns;
}

int netwm_workspace_get_names(char**& names) {
	init_atoms_once();

	/* FIXME: add _NET_SUPPORTING_WM_CHECK and _NET_SUPPORTED ??? */
	XTextProperty wnames;
	XGetTextProperty(fl_display, RootWindow(fl_display, fl_screen), &wnames, _XA_NET_DESKTOP_NAMES);

	/* if wm does not understainds _NET_DESKTOP_NAMES this is not set */
	if(!wnames.nitems || !wnames.value)
		return 0;

	int nsz;

	/*
	 * FIXME: Here should as alternative Xutf8TextPropertyToTextList since
	 * many wm's set UTF8_STRING property. Below is XA_STRING and for UTF8_STRING will fail.
	 */
	if(!XTextPropertyToStringList(&wnames, &names, &nsz)) {
		XFree(wnames.value);
		return 0;
	}

	XFree(wnames.value);
	return nsz;
}

void netwm_workspace_free_names(char **names) {
	E_RETURN_IF_FAIL(names);
	XFreeStringList(names);
}

void netwm_window_set_type(Window win, int t) {
	init_atoms_once();

	/* default */
	Atom type = _XA_NET_WM_WINDOW_TYPE_NORMAL;

	switch(t) {
		case NETWM_WINDOW_TYPE_DESKTOP:       type = _XA_NET_WM_WINDOW_TYPE_DESKTOP; break;
		case NETWM_WINDOW_TYPE_DOCK:          type = _XA_NET_WM_WINDOW_TYPE_DOCK; break; 
		case NETWM_WINDOW_TYPE_TOOLBAR:       type = _XA_NET_WM_WINDOW_TYPE_TOOLBAR; break;
		case NETWM_WINDOW_TYPE_MENU:          type = _XA_NET_WM_WINDOW_TYPE_MENU; break;
		case NETWM_WINDOW_TYPE_UTILITY:       type = _XA_NET_WM_WINDOW_TYPE_UTILITY; break;
		case NETWM_WINDOW_TYPE_SPLASH:        type = _XA_NET_WM_WINDOW_TYPE_SPLASH; break;
		case NETWM_WINDOW_TYPE_DIALOG:        type = _XA_NET_WM_WINDOW_TYPE_DIALOG; break;

		case NETWM_WINDOW_TYPE_DROPDOWN_MENU: type = _XA_NET_WM_WINDOW_TYPE_DROPDOWN_MENU; break;
		case NETWM_WINDOW_TYPE_POPUP_MENU:    type = _XA_NET_WM_WINDOW_TYPE_POPUP_MENU; break;
		case NETWM_WINDOW_TYPE_TOOLTIP:       type = _XA_NET_WM_WINDOW_TYPE_TOOLBAR; break;
		case NETWM_WINDOW_TYPE_NOTIFICATION:  type = _XA_NET_WM_WINDOW_TYPE_NOTIFICATION; break;
		case NETWM_WINDOW_TYPE_COMBO:         type = _XA_NET_WM_WINDOW_TYPE_COMBO; break;
		case NETWM_WINDOW_TYPE_DND:           type = _XA_NET_WM_WINDOW_TYPE_DND; break;
		default: break;
	}

	XChangeProperty(fl_display, win, _XA_NET_WM_WINDOW_TYPE, XA_ATOM, 32, PropModeReplace, 
			(unsigned char*)&type, sizeof(Atom));
}

int netwm_window_get_type(Window win) {
	init_atoms_once();

	Atom real;
	int format;
	unsigned long n, extra;
	unsigned char* prop = 0;

	int status = XGetWindowProperty(fl_display, win, 
			_XA_NET_WM_WINDOW_TYPE, 0L, sizeof(Atom), False, XA_ATOM, &real, &format, &n, &extra, 
			(unsigned char**)&prop);

	if(status != Success || !prop)
		return -1;

	Atom type = *(Atom*)prop;
	XFree(prop);

	if     (type == _XA_NET_WM_WINDOW_TYPE_DESKTOP)       return NETWM_WINDOW_TYPE_DESKTOP;
	else if(type == _XA_NET_WM_WINDOW_TYPE_DOCK)          return NETWM_WINDOW_TYPE_DOCK;
	else if(type == _XA_NET_WM_WINDOW_TYPE_TOOLBAR)       return NETWM_WINDOW_TYPE_TOOLBAR;
	else if(type == _XA_NET_WM_WINDOW_TYPE_MENU)          return NETWM_WINDOW_TYPE_MENU;
	else if(type == _XA_NET_WM_WINDOW_TYPE_UTILITY)       return NETWM_WINDOW_TYPE_UTILITY;
	else if(type == _XA_NET_WM_WINDOW_TYPE_SPLASH)        return NETWM_WINDOW_TYPE_SPLASH;
	else if(type == _XA_NET_WM_WINDOW_TYPE_DIALOG)        return NETWM_WINDOW_TYPE_DIALOG;

	else if(type == _XA_NET_WM_WINDOW_TYPE_DROPDOWN_MENU) return NETWM_WINDOW_TYPE_DROPDOWN_MENU;
	else if(type == _XA_NET_WM_WINDOW_TYPE_POPUP_MENU)    return NETWM_WINDOW_TYPE_POPUP_MENU;
	else if(type == _XA_NET_WM_WINDOW_TYPE_TOOLTIP)       return NETWM_WINDOW_TYPE_TOOLTIP;
	else if(type == _XA_NET_WM_WINDOW_TYPE_NOTIFICATION)  return NETWM_WINDOW_TYPE_NOTIFICATION;
	else if(type == _XA_NET_WM_WINDOW_TYPE_COMBO)         return NETWM_WINDOW_TYPE_COMBO;
	else if(type == _XA_NET_WM_WINDOW_TYPE_DND)           return NETWM_WINDOW_TYPE_DND;

	/* assume it is normal */
	return NETWM_WINDOW_TYPE_NORMAL;
}

void netwm_window_set_strut(Window win, int left, int right, int top, int bottom) {
	init_atoms_once();

	CARD32 strut[4] = { left, right, top, bottom };

	XChangeProperty(fl_display, win, _XA_NET_WM_STRUT, XA_CARDINAL, 32, 
			PropModeReplace, (unsigned char*)&strut, sizeof(CARD32) * 4);
}

int netwm_window_get_all_mapped(Window **windows) {
	init_atoms_once();

	Atom real;
	int format;
	unsigned long n, extra;
	unsigned char* prop = 0;

	int status = XGetWindowProperty(fl_display, RootWindow(fl_display, fl_screen), 
			_XA_NET_CLIENT_LIST, 0L, 0x7fffffff, False, XA_WINDOW, &real, &format, &n, &extra, 
			(unsigned char**)&prop);

	if(status != Success || !prop)
		return -1;

	*windows = (Window*)prop;
	return n;
}

int netwm_window_get_workspace(Window win) {
	E_RETURN_VAL_IF_FAIL(win >= 0, -1);

	init_atoms_once();

	Atom real;
	int format;
	unsigned long n, extra;
	unsigned char* prop = 0;

	int status = XGetWindowProperty(fl_display, win, 
			_XA_NET_WM_DESKTOP, 0L, 0x7fffffff, False, XA_CARDINAL, &real, &format, &n, &extra, 
			(unsigned char**)&prop);

	if(status != Success || !prop)
		return -1;

	unsigned long desk = (unsigned long)(*(long*)prop);
	XFree(prop);

	/* sticky */
	if(desk == 0xFFFFFFFF || desk == 0xFFFFFFFE)
		return -1;

	return desk;
}

int netwm_window_is_manageable(Window win) {
	int t = netwm_window_get_type(win);
	if(t < 0)
		return t;

	if(t == NETWM_WINDOW_TYPE_NORMAL)
		return 1;

	if(t == NETWM_WINDOW_TYPE_DESKTOP || 
	   t == NETWM_WINDOW_TYPE_SPLASH  || 
	   t == NETWM_WINDOW_TYPE_DOCK)
	{
		return 0;
	}

	return 1;
}

char *netwm_window_get_title(Window win) {
	init_atoms_once();

	XTextProperty xtp;
	char		  *title = NULL, *ret = NULL;

	Atom real;
	int format;
	unsigned long n, extra;
	unsigned char* prop = 0;

	int status = XGetWindowProperty(fl_display, win, 
			_XA_NET_WM_NAME, 0L, 0x7fffffff, False, _XA_UTF8_STRING , &real, &format, &n, &extra, 
			(unsigned char**)&prop);

	if(status == Success && prop) {
		title = (char *)prop;
		ret = strdup(title);
		XFree(prop);
	} else {
		/* try WM_NAME */
		if(!XGetWMName(fl_display, win, &xtp))
			return NULL;

		if(xtp.encoding == XA_STRING) {
			ret = strdup((const char*)xtp.value);
		} else {
#if X_HAVE_UTF8_STRING
			char **lst;
			int lst_sz;

			status = Xutf8TextPropertyToTextList(fl_display, &xtp, &lst, &lst_sz);
			if(status == Success && lst_sz > 0) {
				ret = strdup((const char*)*lst);
				XFreeStringList(lst);
			} else {
				ret = strdup((const char*)xtp.value);
			}
#else
			ret = strdup((const char*)tp.value);
#endif
		}

		XFree(xtp.value);
	}

	return ret;
}

Window netwm_window_get_active(void) {
	init_atoms_once();

	Atom real;
	int format;
	unsigned long n, extra;
	unsigned char* prop = 0;

	int status = XGetWindowProperty(fl_display, RootWindow(fl_display, fl_screen), 
			_XA_NET_ACTIVE_WINDOW, 0L, sizeof(Atom), False, XA_WINDOW, &real, &format, &n, &extra, 
			(unsigned char**)&prop);

	if(status != Success || !prop)
		return -1;

	int ret = int(*(long*)prop);
	XFree(prop);

	return (Window)ret;
}

void netwm_window_set_active(Window win) {
	init_atoms_once();	

	XEvent xev;
	memset(&xev, 0, sizeof(xev));
	xev.xclient.type = ClientMessage;
	xev.xclient.serial = 0;
	xev.xclient.send_event = True;
	xev.xclient.window = win;
	xev.xclient.display = fl_display;
	xev.xclient.message_type = _XA_NET_ACTIVE_WINDOW;
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = (long)win;
	xev.xclient.data.l[1] = CurrentTime;

	XSendEvent (fl_display, RootWindow(fl_display, fl_screen), False, 
			SubstructureRedirectMask | SubstructureNotifyMask, &xev);
	XSync(fl_display, True);
}

void netwm_window_maximize(Window win) {
	init_atoms_once();	

	XEvent xev;
	memset(&xev, 0, sizeof(xev));
	xev.xclient.type = ClientMessage;
	xev.xclient.serial = 0;
	xev.xclient.send_event = True;
	xev.xclient.window = win;
	xev.xclient.display = fl_display;
	xev.xclient.message_type = _XA_NET_WM_STATE;
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = 0;
	xev.xclient.data.l[1] = _XA_NET_WM_STATE_MAXIMIZED_HORZ;
	xev.xclient.data.l[2] = _XA_NET_WM_STATE_MAXIMIZED_VERT;

	XSendEvent (fl_display, RootWindow(fl_display, fl_screen), False, 
			SubstructureRedirectMask | SubstructureNotifyMask, &xev);
	XSync(fl_display, True);
}

void netwm_window_close(Window win) {
	init_atoms_once();	

	XEvent xev;
	memset(&xev, 0, sizeof(xev));
	xev.xclient.type = ClientMessage;
	xev.xclient.serial = 0;
	xev.xclient.send_event = True;
	xev.xclient.window = win;
	xev.xclient.display = fl_display;
	xev.xclient.message_type = _XA_NET_CLOSE_WINDOW;
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = (long)win;
	xev.xclient.data.l[1] = CurrentTime;

	XSendEvent (fl_display, RootWindow(fl_display, fl_screen), False, 
			SubstructureRedirectMask | SubstructureNotifyMask, &xev);
	XSync(fl_display, True);
}

void wm_window_ede_restore(Window win) {
	init_atoms_once();	

	XEvent xev;
	memset(&xev, 0, sizeof(xev));
	xev.xclient.type = ClientMessage;
	xev.xclient.serial = 0;
	xev.xclient.send_event = True;
	xev.xclient.window = win;
	xev.xclient.display = fl_display;
	xev.xclient.message_type = _XA_EDE_WM_ACTION;
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = _XA_EDE_WM_RESTORE_SIZE;
	xev.xclient.data.l[1] = CurrentTime;

	XSendEvent (fl_display, RootWindow(fl_display, fl_screen), False, 
			SubstructureRedirectMask | SubstructureNotifyMask, &xev);
	XSync(fl_display, True);
}

WmStateValue wm_window_get_state(Window win) {
	init_atoms_once();

	Atom real;
	int format;
	unsigned long n, extra;
	unsigned char* prop = 0;

	int status = XGetWindowProperty(fl_display, win,
			_XA_WM_STATE, 0L, sizeof(Atom), False, _XA_WM_STATE, &real, &format, &n, &extra, 
			(unsigned char**)&prop);

	if(status != Success || !prop)
		return WM_WINDOW_STATE_NONE;

	WmStateValue ret = WmStateValue(*(long*)prop);
	XFree(prop);

	return ret;
}

void wm_window_set_state(Window win, WmStateValue state) {
	E_RETURN_IF_FAIL((int)state > 0);

	if(state == WM_WINDOW_STATE_WITHDRAW) {
		XWithdrawWindow(fl_display, win, fl_screen);
		XSync(fl_display, True);
	} else if(state == WM_WINDOW_STATE_ICONIC) {
		XIconifyWindow(fl_display, win, fl_screen);
		XSync(fl_display, True);
	}
	
	/* nothing for WM_WINDOW_STATE_NORMAL */
}

EDELIB_NS_END
