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
#include <FL/Fl_Window.h>
#include "XSettingsClient.h"

EDELIB_NS_BEGIN

class EDELIB_API Window : public Fl_Window {
	private:
		XSettingsClient xs;
		void init(void);

	public:
		Window(int X, int Y, int W, int H, const char* l = 0);
		Window(int W, int H, const char* l = 0);
		virtual ~Window();
		virtual void show(void);
};

EDELIB_NS_END

#endif
