/*
 * $Id$
 *
 * Window class
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

#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "XSettingsClient.h"
#include "IconTheme.h"
#include <FL/Fl_Double_Window.H>

EDELIB_NS_BEGIN

/**
 * \ingroup widgets
 * \enum WindowComponents
 * \brief Components used to be loaded with the window
 */
enum WindowComponents {
	WIN_INIT_NONE       = (1 << 1),                               ///< Do not load anything except XSETTINGS code
	WIN_INIT_ICON_THEME = (1 << 2),                               ///< Load IconTheme code
	WIN_INIT_IMAGES     = (1 << 3),                               ///< Call fl_register_images
	WIN_INIT_ALL        = (WIN_INIT_ICON_THEME | WIN_INIT_IMAGES) ///< Load above
};

#ifndef SKIP_DOCS
typedef bool (WindowXSettingsCallback)(const char* name, XSettingsAction action, 
		const XSettingsSetting* setting, void* data);

typedef void (WindowSettingsCallback)(void* data);
#endif

/**
 * \ingroup widgets
 * \class Window
 * \brief Window class
 *
 * This is Window class similar to FLTK's Fl_Window and Fl_Double_Window with addition of icon themes,
 * xsettings, dialog icons setup and image initialization code, often called prior program startup.
 *
 * It will also clean loaded data (e.g. call IconTheme::shutdown()).
 *
 * Contrary to FLTK's Fl_Window and Fl_Double_Window which are separate classes, this class can be
 * both of them (actually you can chose will window be single buffered or double buffered). If member
 * single_buffer() is set to true, window will behave as Fl_Window, if not (default), window will
 * be as Fl_Double_Window.
 *
 * Make sure to call single_buffer() <em>before</em> show() if you want to change single/double buffering
 * scheme.
 */
class EDELIB_API Window : public Fl_Double_Window {
private:
	bool inited;
	bool sbuffer;
	int  loaded_components;
	XSettingsClient xs;
	unsigned long pref_atom; /* TODO: this should be Atom type */
	unsigned int  pref_uid;

	WindowXSettingsCallback* xs_cb;
	WindowXSettingsCallback* xs_cb_old;
	void* xs_cb_data;

	WindowSettingsCallback* s_cb;
	void* s_cb_data;

	const char* const* icon_pixmap;

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
	 * Set a window icon (icon that will be shown in titlebar). To allow compatibility
	 * with FLTK, this function must be called <em>before</em> show() or icon will not
	 * be shown.
	 */
	void window_icon(const char* const * pix) { icon_pixmap = pix; }

	/**
	 * Get a window icon
	 */
	const char* const* window_icon(void) { return icon_pixmap; }

	/**
	 * Show a window
	 */
	virtual void show(void);

	/**
	 * Show a window
	 */
	virtual void show(int argc, char** argv) { Fl_Window::show(argc, argv); }

	/**
	 * Set window either to single or double buffered. Only valid if called before show().
	 */
	void single_buffer(bool s) { sbuffer = s; }

	/**
	 * Returns true if window is single buffered
	 */
	bool single_buffer(void) { return sbuffer; }

	/**
	 * Returns true if window is double buffered
	 */
	bool double_buffer(void) { return !single_buffer(); }

	/**
	 * Flush window content. If you inherit this class and re-implement flush(),
	 * make sure you call this flush() not the one from Fl_Window or Fl_Double_Window.
	 */
	virtual void flush(void) { 
		if(!sbuffer) 
			Fl_Double_Window::flush();
		else 
			Fl_Window::flush(); 
	}

	/**
	 * Resize window. If you inherit this class and re-implement resize(),
	 * make sure you call this resize() not the one from Fl_Window or Fl_Double_Window.
	 */
	virtual void resize(int X, int Y, int W, int H) {
		if(!sbuffer)
			Fl_Double_Window::resize(X, Y, W, H);
		else
			Fl_Window::resize(X, Y, W, H);
	}

	/**
	 * Hide window. If you inherit this class and re-implement hide(),
	 * make sure you call this hide() not the one from Fl_Window or Fl_Double_Window.
	 */
	virtual void hide(void) {
		if(!sbuffer)
			Fl_Double_Window::hide();
		else
			Fl_Window::hide();
	}
};

/**
 * \ingroup widgets
 *
 * This function is intended to be used inside inherited show() member (from Fl_Window familly)
 * and do the same job as Fl_X::make_xid(). The main difference is that it will call before_map_func()
 * (if given) before window is actually mapped. 
 *
 * This is usefull for cases when window data must exists (when FLTK creates them) so some properties
 * can be set before mapping on the screen (actually these properties can be set after window was mapped 
 * but many window managers, panels, etc. will not be notified correctly and will not use them; of course
 * not all properies must be set before window was mapped, but for those that must, this function is intended).
 *
 * In short, if you have not clue what I was talking about, then you don't need this function at all ;-).
 */
void create_window_xid(Fl_Window* win, void (*before_map_func)(Fl_Window*) = NULL, int background_pixel = -1);

EDELIB_NS_END

#endif
