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

#ifndef __XSETTINGSCLIENT_H__
#define __XSETTINGSCLIENT_H__

#include "XSettingsCommon.h"

EDELIB_NS_BEGIN

enum XSettingsAction {
	XSETTINGS_ACTION_NEW = 0,
	XSETTINGS_ACTION_CHANGED,
	XSETTINGS_ACTION_DELETED
};

typedef void (*XSettingsCallback)(const char* name, XSettingsAction action, XSettingsSetting* setting, void* data);

class XSettingsClient {
	private:
		XSettingsData* client_data;
		XSettingsCallback settings_cb;
		void* settings_cb_data;

		void check_manager_window(void);
		void read_settings(void);

	public:
		XSettingsClient();
		~XSettingsClient();

		bool init(XSettingsCallback cb, void* data = 0);
		void callback(XSettingsCallback cb, void* data = 0);
		int process_xevent(const XEvent* xev);

		void set(const char* name, int val);
		void set(const char* name, const char* val);
		void set(const char* name, unsigned short red, unsigned short green, unsigned short blue, unsigned short alpha);
};

EDELIB_NS_END

#endif
