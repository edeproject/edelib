/*
 * $Id$
 *
 * Client part of XSETTINGS protocol
 * Part of edelib.
 * Based on implementation from Owen Tylor, copyright (c) 2001 Red Hat, inc.
 * Copyright (c) 2005-2007 EDE Authors.
 *
 * This program is licensed under terms of the 
 * GNU General Public License version 2 or newer.
 * See COPYING for details.
 */

#include <edelib/XSettingsClient.h>
#include <edelib/Debug.h>
#include <stdio.h>  // puts
#include <string.h> // strcmp
#include <limits.h> // LONG_MAX
#include <FL/x.h>

EDELIB_NS_BEGIN

static int ignore_xerrors(Display* display, XErrorEvent* xev) {
	return True;
}

static void notify_changes(XSettingsData* client, XSettingsList* old_settings, XSettingsCallback callback, void* cb_data) {
	if(!callback)
		return;

	XSettingsList* old_iter = old_settings;
	XSettingsList* new_iter = client->settings;

	while(old_iter || new_iter) {
		int cmp;

		if(old_iter && new_iter)
			cmp = strcmp(old_iter->setting->name, new_iter->setting->name);
		else if(old_iter)
			cmp = -1;
		else
			cmp = 1;

		if(cmp < 0)
			callback(old_iter->setting->name, XSETTINGS_ACTION_DELETED, NULL, cb_data);
		else if(cmp == 0) {
			if(!xsettings_setting_equal(old_iter->setting, new_iter->setting))
				callback(old_iter->setting->name, XSETTINGS_ACTION_CHANGED, new_iter->setting, cb_data);
		} else
			callback(new_iter->setting->name, XSETTINGS_ACTION_NEW, new_iter->setting, cb_data);
		
		if(old_iter)
			old_iter = old_iter->next;
		if(new_iter)
			new_iter = new_iter->next;
	}
}

XSettingsClient::XSettingsClient() : client_data(NULL), settings_cb(NULL), settings_cb_data(NULL) { }

XSettingsClient::~XSettingsClient() { 
	clear();
}

void XSettingsClient::check_manager_window(void) {
	EASSERT(client_data != NULL);

	XGrabServer(client_data->display); // required by spec

	client_data->manager_win = XGetSelectionOwner(client_data->display, client_data->selection_atom);
	if(client_data->manager_win)
		XSelectInput(client_data->display, client_data->manager_win, PropertyChangeMask | StructureNotifyMask);

	XUngrabServer(client_data->display); // required by spec
	XFlush(client_data->display);

	read_settings();
}

void XSettingsClient::read_settings(void) {
	EASSERT(client_data != NULL);

	Atom type;
	int format;
	unsigned long n_items, bytes_after;
	unsigned char *data;
	int result;

	int (*old_handler)(Display*, XErrorEvent*);

	XSettingsList* old_settings = client_data->settings;
	client_data->settings = NULL;

	if(client_data->manager_win) {
		old_handler = XSetErrorHandler(ignore_xerrors);

		result = XGetWindowProperty(client_data->display, client_data->manager_win, client_data->xsettings_atom,
				0, LONG_MAX, False, client_data->xsettings_atom, 
				&type, &format, &n_items, &bytes_after, (unsigned char**)&data);

		XSetErrorHandler(old_handler);

		if(result == Success && type != None) {
			if(type != client_data->xsettings_atom)
				puts("Invalid type for XSETTINGS property");
			else if(format != 8)
				printf("Invalid format for XSETTINGS property %d\n", format);
			else {
				// parse settings
				client_data->settings = xsettings_decode(data, n_items, &client_data->serial);
			}

			XFree(data);
		}
	}

	notify_changes(client_data, old_settings, settings_cb, settings_cb_data);
	xsettings_list_free(old_settings);
}

bool XSettingsClient::init(Display* dpy, int screen, XSettingsCallback cb, void* data) { 
	if(client_data)
		return true;

	callback(cb, data);

	client_data = new XSettingsData;
	client_data->display = dpy;
	client_data->screen = screen;
	client_data->manager_win = None;
	client_data->settings = NULL;
	client_data->serial = 0;

	char buff[256];
	snprintf(buff, sizeof(buff)-1, "_XSETTINGS_S%d", fl_screen);

	client_data->selection_atom = XInternAtom(client_data->display, buff, False);
	client_data->manager_atom = XInternAtom(client_data->display, "MANAGER", False);
	client_data->xsettings_atom = XInternAtom(client_data->display, "_XSETTINGS_SETTINGS", False);

	// use StructureNotify to get MANAGER events
	XWindowAttributes attrs;
	Window root_win = RootWindow(client_data->display, client_data->screen);
	XGetWindowAttributes(client_data->display, root_win, &attrs);
	XSelectInput(client_data->display, root_win, attrs.your_event_mask | StructureNotifyMask);

	check_manager_window();

	return true;
}

void XSettingsClient::clear(void) {
	if(!client_data)
		return;

	xsettings_list_free(client_data->settings);
	/* delete client_data->settings; */
	delete client_data;
	client_data = NULL;
}

void XSettingsClient::callback(XSettingsCallback cb, void* data) {
	settings_cb = cb;
	settings_cb_data = data;
}

int XSettingsClient::process_xevent(const XEvent* xev) {
	if(xev->xany.window == RootWindow(client_data->display, client_data->screen)) {
		if(xev->xany.type == ClientMessage && xev->xclient.message_type == client_data->manager_atom &&
				(unsigned int)xev->xclient.data.l[1] == client_data->selection_atom) {
			check_manager_window();
			return True;
		}
	} else if(xev->xany.window == client_data->manager_win) {
		if(xev->xany.type == DestroyNotify) {
			puts("Manager quit");
			check_manager_window();
			return True;
		} else if(xev->xany.type == PropertyNotify) {
			read_settings();
			return True;
		}
	}

	return False;
}

void XSettingsClient::set(const char* name, int val) {
	EASSERT(client_data != NULL && "init() must be called before this function");

	if(!client_data->manager_win)
		return;

	XSettingsSetting setting;
	setting.name = (char*)name;
	setting.type = XSETTINGS_TYPE_INT;
	setting.data.v_int = val;

	xsettings_manager_set_setting(client_data, &setting);
}

void XSettingsClient::set(const char* name, const char* val) {
	EASSERT(client_data != NULL && "init() must be called before this function");

	if(!client_data->manager_win)
		return;

	XSettingsSetting setting;
	setting.name = (char*)name;
	setting.type = XSETTINGS_TYPE_STRING;
	setting.data.v_string = (char*)val;

	xsettings_manager_set_setting(client_data, &setting);
}

void XSettingsClient::set(const char* name, unsigned short red, unsigned short green, 
		unsigned short blue, unsigned short alpha) {
	EASSERT(client_data != NULL && "init() must be called before this function");

	if(!client_data->manager_win)
		return;

	XSettingsSetting setting;
	setting.name = (char*)name;
	setting.type = XSETTINGS_TYPE_COLOR;
	setting.data.v_color.red = red;
	setting.data.v_color.green = green;
	setting.data.v_color.blue = blue;
	setting.data.v_color.alpha = alpha;

	xsettings_manager_set_setting(client_data, &setting);
}

void XSettingsClient::manager_notify(void) {
	EASSERT(client_data != NULL && "init() must be called before this function");

	if(!client_data->manager_win)
		return;

	xsettings_manager_notify(client_data);
}

EDELIB_NS_END
