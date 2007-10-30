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
#include <FL/Fl_Window.h>

EDELIB_NS_BEGIN

/**
 * \class Window
 * \brief Window class
 *
 * This is Window class similar to FLTK's Fl_Window, but with XSETTINGS support.
 * \todo finish documentation
 */
class EDELIB_API Window : public Fl_Window {
	private:
		bool inited;
		XSettingsClient xs;
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
		 * \todo finish
		 */
		void init(void);

		/**
		 * Destructor
		 */
		virtual ~Window();

		/**
		 * Shows a window
		 */
		virtual void show(void);
};

EDELIB_NS_END

#endif
