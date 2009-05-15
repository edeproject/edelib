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

typedef void (WindowForeignCallback)(void* data);
#endif

/**
 * \ingroup widgets
 * \class Window
 * \brief Window class
 *
 * This is Window class similar to FLTK's Fl_Window and Fl_Double_Window with addition of icon themes,
 * XSETTINGS protocol, dialog icons setup and image initialization code, often called prior program startup.
 *
 * It will also clean loaded data (e.g. call IconLoader::shutdown()), automatically when window is closed.
 *
 * Contrary to the FLTK's Fl_Window and Fl_Double_Window which are separate classes, this class can be
 * both of them (actually you can chose will window be single buffered or double buffered). If member
 * single_buffer() is set to true, window will behave as Fl_Window, if not (default), window will
 * be as Fl_Double_Window.
 *
 * Make sure to call single_buffer() before show() if you want to change single/double buffering scheme.
 *
 * Window implements the following XSETTINGS keys:
 *  - Fltk/Background - set FL_BACKGROUND color
 *  - Fltk/Background2 - set FL_BACKGROUND2 color
 *  - Fltk/Foreground - set FL_FOREGROUND color
 *  - Fltk/FontSize - set font size
 *  - Net/IconThemeName - load icon theme with the given name
 *
 * \note Due some FLTK issues, when font size was changed (via Fltk/FontSize), window will not be redrawn
 * (actually, it can't be redrawn), so window has to be closed and opened again.
 */
class EDELIB_API Window : public Fl_Double_Window {
private:
	bool sbuffer;
	int  loaded_components;

	XSettingsClient*         xs;

	WindowXSettingsCallback* xs_cb;
	WindowXSettingsCallback* xs_cb_old;
	void*                    xs_cb_data;

	const char* const*       icon_pixmap;

	unsigned int             wid;
	WindowForeignCallback*   foreign_cb;
	void*                    foreign_cb_data;

	void init(int component);
public:
	/**
	 * Constructor
	 */
	Window(int X, int Y, int W, int H, const char* l = 0, int component = WIN_INIT_ALL);

	/**
	 * Constructor
	 */
	Window(int W, int H, const char* l = 0, int component = WIN_INIT_ALL);

	/**
	 * Destructor
	 */
	virtual ~Window();

	/**
	 * Returns loaded parts given in WindowComponents
	 */
	int component(void) { return loaded_components; }

	/**
	 * Register callback for changes via XSETTINGS protocol. Optional, <em>data</em> parameter will
	 * be passed to the callback.
	 *
	 * If callback function returns <em>true</em>, window will be redrawn.
	 */
	void xsettings_callback(WindowXSettingsCallback& cb, void* data = NULL) { xs_cb = cb; xs_cb_data = data; }

	/**
	 * Returns callback used for XSETTINGS protocol
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

	/**
	 * Setup foreign callback. Foreign callbacks allows to call registered functions
	 * in (or from) external programs, by using X messaging system. 
	 *
	 * To do so, window (it has to be Window class or inherited one), if wants to allow
	 * external calls, has to register <em>window ID</em>. This should be unique, integer
	 * number, known to the external caller.
	 *
	 * After setting ID and callback via foreign_callback(), external caller will use 
	 * do_foreign_callback(ID) to call it.
	 *
	 * Foreign callbacks can be used to instruct application to re-read it's configuration
	 * file, after that file was changed by different program.
	 */
	void foreign_callback(WindowForeignCallback* cb, void* data = NULL) { 
		foreign_cb = cb; 
		foreign_cb_data = data; 
	}

	/**
	 * Setup data for foreign callback
	 */
	void foreign_callback_data(void* data) { foreign_cb_data = data; }

	/**
	 * Returns pointer to foreign callback function
	 */
	WindowForeignCallback* foreign_callback(void) { return foreign_cb; }

	/**
	 * Return additional data for foreign callback. If data wasn't set previously, returned
	 * value will be NULL
	 */
	void* foreign_callback_data(void) { return foreign_cb_data; }

	/**
	 * Setup window ID (used by foreign callbacks)
	 */
	void window_id(unsigned int id) { wid = id; }

	/**
	 * Returns window ID (used by foreign callbacks)
	 */
	unsigned int window_id(void) { return wid; }

	/**
	 * Call registered foreign callback, by window id
	 */
	static void do_foreign_callback(unsigned int id);

	/**
	 * Call foreign callbacks of all windows, no matter what ID they set.
	 * This is useful when all apps should call callbacks in the same time, due e.g. some 
	 * global updates or similar
	 */
	static void do_foreign_callback_for_all(void) { do_foreign_callback(0); }
};

EDELIB_NS_END

#endif
