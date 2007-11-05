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

#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "econfig.h"
#include "XSettingsClient.h"
#include "IconTheme.h"
#include <FL/Fl_Window.h>

EDELIB_NS_BEGIN

/**
 * \enum WindowComponents
 * \brief Components used to be loaded with the window
 */
enum WindowComponents {
	WIN_INIT_NONE       = (1 << 1),   ///< Do not load anything except XSETTINGS code
	WIN_INIT_ICON_THEME = (1 << 2),   ///< Load IconTheme code
	WIN_INIT_IMAGES     = (1 << 3),   ///< Call fl_register_images
	WIN_INIT_ALL        = (WIN_INIT_ICON_THEME | WIN_INIT_IMAGES) ///< Load above
};

#ifndef SKIP_DOCS
typedef bool (WindowXSettingsCallback)(const char* name, XSettingsAction action, 
		const XSettingsSetting* setting, void* data);

typedef void (WindowSettingsCallback)(void* data);
#endif

/**
 * \class Window
 * \brief Window class
 *
 * This is Window class similar to FLTK's Fl_Window with addition of icon themes,
 * xsettings, dialog icons setup and image initialization code, often called prior program startup.
 *
 * It will also clean loaded data (e.g. call IconTheme::shutdown()).
 */
class EDELIB_API Window : public Fl_Window {
	private:
		bool inited;
		int  loaded_components;
		XSettingsClient xs;
		unsigned long pref_atom; /* TODO: this should be Atom type */
		unsigned int  pref_uid;

		WindowXSettingsCallback* xs_cb;
		WindowXSettingsCallback* xs_cb_old;
		void* xs_cb_data;

		WindowSettingsCallback* s_cb;
		void* s_cb_data;

	public:
		/**
		 * Constructor
		 */
		Window(int X, int Y, int W, int H, const char* l = 0);

		/**
		 * Constructor
		 */
		Window(int W, int H, const char* l = 0);

		/**
		 * Destructor
		 */
		virtual ~Window();

		/**
		 * Load one or all components. Note that this function must be called
		 * before show() (event if WIN_INIT_NONE is given) since will load XSETTINGS code
		 */
		void init(int component = WIN_INIT_ALL);

		/**
		 * Set UID (unique ID) for this window
		 */
		void settings_uid(unsigned int uid) { pref_uid = uid; }

		/**
		 * Get window UID
		 */
		unsigned int settings_uid(void) { return pref_uid; }

		/**
		 * Register callback for changes. Changes will be called with Window::update_settings()
		 * \param cb is callback function
		 * \param data is data passed to the callback
		 */
		void settings_callback(WindowSettingsCallback& cb, void* data = NULL) { s_cb = cb; s_cb_data = data; }

		/**
		 * Returns callback used for settings
		 */
		WindowSettingsCallback* settings_callback(void) { return s_cb; }

		/**
		 * Returns data assigned that should be passed to the settings_callback
		 */
		void* settings_callback_data(void) { return s_cb_data; }

		/**
		 * Returns atom used for settings. Internaly used by Window class
		 */
		unsigned long preferences_atom(void) { return pref_atom; }
	
		/**
		 * Update settings for window with give uid
		 * \param uid window which will update it's settings
		 */
		static void update_settings(unsigned int uid);

		/**
		 * Returns pointer to XSettingsClient member. init() must be called
		 * prior this function or assertion will be triggered
		 */
		XSettingsClient* xsettings(void);

		/**
		 * Register callback for changes in XSETTINGS
		 * \param cb is callback function
		 * \param data is data passed to the callback
		 */
		void xsettings_callback(WindowXSettingsCallback& cb, void* data = NULL) { xs_cb = cb; xs_cb_data = data; }

		/**
		 * Returns callback used for XSETTINGS
		 */
		WindowXSettingsCallback* xsettings_callback(void) { return xs_cb; }

		/**
		 * Returns data assigned that should be passed to the XSETTINGS callback
		 */
		void* xsettings_callback_data(void) { return xs_cb_data; }

		/**
		 * This function is meant to be used to temporary disable XSETTINGS callback.
		 * This is often used to prevent callback be called twice when is
		 * set() called from XSettingsClient.
		 */
		void pause_xsettings_callback(void) { xs_cb_old = xs_cb; xs_cb = NULL; }

		/**
		 * Restore previously disabled callback. Note that this function <em>must</em>
		 * be called <em>after</em> pause_xsettings_callback() or it will disable
		 * callback completely.
		 */
		void restore_xsettings_callback(void) { xs_cb = xs_cb_old; }

		/**
		 * Show a window
		 */
		virtual void show(void);
};

EDELIB_NS_END

#endif
