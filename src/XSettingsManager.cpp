/*
 * $Id$
 *
 * Manager part of XSETTINGS protocol
 * Part of edelib.
 * Based on implementation from Owen Tylor, copyright (c) 2001 Red Hat, inc.
 * Copyright (c) 2005-2007 EDE Authors.
 *
 * This program is licensed under terms of the 
 * GNU General Public License version 2 or newer.
 * See COPYING for details.
 */

#include <edelib/XSettingsManager.h>
#include <edelib/Debug.h>
#include <stdio.h> // sprintf

EDELIB_NS_BEGIN

struct TimeStampInfo {
	Window window;
	Atom timestamp_prop_atom;
};

Bool timestamp_predicate(Display* dpy, XEvent* xev, XPointer arg) {
	TimeStampInfo* info = (TimeStampInfo*)arg;

	if(xev->type == PropertyNotify &&
		xev->xproperty.window == info->window &&
		xev->xproperty.atom == info->timestamp_prop_atom) {
		return True;
	}

	return False;
}

Time get_server_time(Display* dpy, Window win) {
	unsigned char c = 'a';
	TimeStampInfo info;
	XEvent xev;

	info.timestamp_prop_atom = XInternAtom(dpy, "_TIMESTAMP_PROP", False);
	info.window = win;

	XChangeProperty(dpy, win, info.timestamp_prop_atom, info.timestamp_prop_atom,
			8, PropModeReplace, &c, 1);

	XIfEvent(dpy, &xev, timestamp_predicate, (XPointer)&info);
	return xev.xproperty.time;
}

XSettingsManager::XSettingsManager() : manager_data(NULL) { }

XSettingsManager::~XSettingsManager() {
	clear();
}

bool XSettingsManager::init(Display* dpy, int screen) {
	if(manager_data)
		return true;

	manager_data = new XSettingsData;
	manager_data->display = dpy;
	manager_data->screen = screen;
	manager_data->manager_win = None;
	manager_data->settings = NULL;

	char buff[256];
	sprintf(buff, "_XSETTINGS_S%d", manager_data->screen);

	manager_data->selection_atom = XInternAtom(manager_data->display, buff, False);
	manager_data->xsettings_atom = XInternAtom(manager_data->display, "_XSETTINGS_SETTINGS", False);
	manager_data->manager_atom = XInternAtom(manager_data->display, "MANAGER", False);

	manager_data->serial = 0;

	Window root = RootWindow(manager_data->display, manager_data->screen);
	manager_data->manager_win = XCreateSimpleWindow(manager_data->display, root, 0, 0, 10, 10, 0,
			WhitePixel(manager_data->display, manager_data->screen),
			WhitePixel(manager_data->display, manager_data->screen));

	XSelectInput(manager_data->display, manager_data->manager_win, PropertyChangeMask);
	Time timestamp = get_server_time(manager_data->display, manager_data->manager_win);

	XSetSelectionOwner(manager_data->display, manager_data->selection_atom, manager_data->manager_win, timestamp);

	// check if we got ownership
	if(XGetSelectionOwner(manager_data->display, manager_data->selection_atom) == manager_data->manager_win) {
		XClientMessageEvent xev;
		xev.type = ClientMessage;
		xev.window = root;
		xev.message_type = manager_data->manager_atom;
		xev.format = 32;
		xev.data.l[0] = timestamp;
		xev.data.l[1] = manager_data->selection_atom;
		xev.data.l[2] = manager_data->manager_win;
		xev.data.l[3] = 0; // manager specific data
		xev.data.l[4] = 0; // manager specific data

		XSendEvent(manager_data->display, root, False, StructureNotifyMask, (XEvent*)&xev);
		return true;
	}

	/*
	 * allow init() be called again
	 * FIXME: really should this exists here ?
	 */
	clear();
	return false;
}

void XSettingsManager::clear(void) {
	if(!manager_data)
		return;

	XDestroyWindow(manager_data->display, manager_data->manager_win);
	manager_data->manager_win = None;

	xsettings_list_free(manager_data->settings);
	/* delete manager_data->settings; */
	delete manager_data;
	manager_data = NULL;
}

/* static */
bool XSettingsManager::manager_running(Display* dpy, int screen) {
	char buff[256];
	sprintf(buff, "_XSETTINGS_S%d", screen);

	Atom selection = XInternAtom(dpy, buff, False);
	if(XGetSelectionOwner(dpy, selection))
		return true;
	return false;
}

bool XSettingsManager::should_terminate(const XEvent* xev) {
	EASSERT(manager_data != NULL && "init() must be called before this function");

	if(xev->xany.window == manager_data->manager_win &&
		xev->xany.type == SelectionClear &&
		xev->xselectionclear.selection == manager_data->selection_atom) {
		return true;
	}

	return false;
}

void XSettingsManager::set(const char* name, int val) {
	EASSERT(manager_data != NULL && "init() must be called before this function");

	if(!manager_data->manager_win)
		return;

	XSettingsSetting setting;
	setting.name = (char*)name;
	setting.type = XSETTINGS_TYPE_INT;
	setting.data.v_int = val;

	xsettings_manager_set_setting(manager_data, &setting);
	/* xsettings_manager_notify(manager_data); */
}

void XSettingsManager::set(const char* name, const char* val) {
	EASSERT(manager_data != NULL && "init() must be called before this function");

	if(!manager_data->manager_win)
		return;

	XSettingsSetting setting;
	setting.name = (char*)name;
	setting.type = XSETTINGS_TYPE_STRING;
	setting.data.v_string = (char*)val;

	xsettings_manager_set_setting(manager_data, &setting);
	/* xsettings_manager_notify(manager_data); */
}

void XSettingsManager::set(const char* name, 
		unsigned short red, unsigned short green, unsigned short blue, unsigned short alpha) {
	EASSERT(manager_data != NULL && "init() must be called before this function");

	if(!manager_data->manager_win)
		return;

	XSettingsSetting setting;
	setting.name = (char*)name;
	setting.type = XSETTINGS_TYPE_COLOR;
	setting.data.v_color.red = red;
	setting.data.v_color.green = green;
	setting.data.v_color.blue = blue;
	setting.data.v_color.alpha = alpha;

	xsettings_manager_set_setting(manager_data, &setting);
	/* xsettings_manager_notify(manager_data); */
}

void XSettingsManager::notify(void) {
	EASSERT(manager_data != NULL && "init() must be called before this function");

	if(!manager_data->manager_win)
		return;

	xsettings_manager_notify(manager_data);
}

EDELIB_NS_END
