/*
 * $Id$
 *
 * Client part of XSETTINGS protocol
 * Based on implementation from Owen Tylor, copyright (c) 2001 Red Hat, inc.
 * Copyright (c) 2005-2007 edelib authors
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

#ifndef __XSETTINGSCLIENT_H__
#define __XSETTINGSCLIENT_H__

#include "XSettingsCommon.h"

EDELIB_NS_BEGIN

/**
 * \enum XSettingsAction
 * \brief Flags telling what was done with setting
 */
enum XSettingsAction {
	XSETTINGS_ACTION_NEW = 0,   ///< New setting added
	XSETTINGS_ACTION_CHANGED,   ///< One of the settings changed
	XSETTINGS_ACTION_DELETED    ///< One of the settings deleted
};

#ifndef SKIP_DOCS
typedef void (*XSettingsCallback)(const char* name, XSettingsAction action, XSettingsSetting* setting, void* data);
#endif

/**
 * \class XSettingsClient
 * \brief Client part of XSETTINGS protocol
 *
 * This class is used to query XSETTINGS data from manager. Due XSETTINGS protocol
 * requirements, manager must already be running. To see differences between official
 * protocol and current implementation, please see XSettingsData documentation.
 *
 * Before query data, init() must be called. init() will register needed data and
 * (optionaly) register callback for listening changes. This callback will be triggered when 
 * one of the settings are changed on manager side so application can update needed data.
 *
 * Also to get changes via callback, process_xevent() must be placed in loop with X events
 * so XSettingsClient can capture needed events and process them.
 *
 * This is simple example how stuff should be set up:
 * \code
 *   void xsettings_cb(const char* name, XSettingsAction action, XSettingsSetting* setting, void* data) {
 *      if(strcmp(name, "Test/MySetting") == 0) {
 *         switch(action) {
 *            case XSETTINGS_ACTION_NEW:
 *               // added setting
 *               if(setting->type == XSETTING_TYPE_INT)
 *                  printf("Added %s with value %i\n", setting->name, setting->data.v_int);
 *               if(setting->type == XSETTING_TYPE_STRING)
 *                  printf("Added %s with value %i\n", setting->name, setting->data.v_string);
 *               // and so on...
 *               break;
 *            case XSETTINGS_ACTION_CHANGED:
 *               // changed setting
 *               // inspect them as for XSETTINGS_ACTION_NEW
 *               break;
 *            case XSETTINGS_ACTION_DELETED:
 *               // deleted setting
 *               // inspect them as for XSETTINGS_ACTION_NEW
 *               break;
 *         }
 *      }
 *      return;
 *  }
 *
 *  // in some function...
 *
 *  XSettingsClient client;
 *  if(!client.init(fl_display, fl_screen, xsettings_cb)) {
 *    puts("Unable to init XSETTINGS client");
 *    return;
 *  }
 *
 *  XEvent ev;
 *  while(1) {
 *    XNextEvent(display, &ev);
 *    return client.process_xevent(&ev);
 *  }
 * \endcode
 *
 * init() function intentionally have (optional) callback parameter since after calling init(),
 * callback can be triggered immediately if manager is running so XSettingsClient can pick up
 * currently stored settings from it. Alternatively there is callback() function which can be used
 * to change callback during runtime.
 *
 * Calling callback() before init() is the same as calling init() with callback function.
 *
 * Note that if callback() is called <em>after</em> init(), settings will not be queried from manager
 * until one of the settings is changed on manager side.
 */
class EDELIB_API XSettingsClient {
private:
	XSettingsData* client_data;
	XSettingsCallback settings_cb;
	void* settings_cb_data;

	void check_manager_window(void);
	void read_settings(void);

	XSettingsClient(const XSettingsClient&);
	XSettingsClient& operator=(const XSettingsClient&);

public:
	/**
	 * Empty constructor
	 */
	XSettingsClient();

	/**
	 * Clears internal data
	 */
	~XSettingsClient();

	/**
	 * Init needed communication parts.
	 * \return false if failed, true otherwise
	 * \param dpy is display
	 * \param screen is screen
	 * \param cb is callback parameter
	 * \param data is data passed to the callback function
	 */
	bool init(Display* dpy, int screen, XSettingsCallback cb = 0, void* data = 0);

	/**
	 * Clears internal data. init() must be called agan if other
	 * functions are going to be called
	 */
	void clear(void);

	/**
	 * Checks if manager is running
	 * \return true if does otherwise false
	 * \param dpy is display
	 * \param screen is screen
	 */
	static bool manager_running(Display* dpy, int screen);

	/**
	 * Set callback for listening changes
	 * \param cb is callback parameter
	 * \param data is data passed to the callback function
	 */
	void callback(XSettingsCallback cb, void* data = 0);

	/**
	 * This function should be placed in loop with X events
	 */
	int process_xevent(const XEvent* xev);

	/**
	 * Set integer value on manager
	 * \param name is setting name
	 * \param val is int value
	 */
	void set(const char* name, int val);

	/**
	 * Set string (char*) value on manager. Data will be copied
	 * \param name is setting name
	 * \param val is string value
	 */
	void set(const char* name, const char* val);

	/**
	 * Set RGBA color values on manager
	 * \param name is setting name
	 * \param red is red component
	 * \param green is green component
	 * \param blue is blue component
	 * \param alpha is alpha component
	 */
	void set(const char* name, unsigned short red, unsigned short green, unsigned short blue, unsigned short alpha);

	/**
	 * Send changed data to manager. This should be called aftear set()
	 */
	void manager_notify(void);
};

EDELIB_NS_END

#endif
