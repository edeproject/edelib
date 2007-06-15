/*
 * $Id$
 *
 * X11 options setter
 * Part of edelib.
 * Copyright (c) 2005-2007 EDE Authors.
 *
 * This program is licenced under terms of the 
 * GNU General Public Licence version 2 or newer.
 * See COPYING for details.
 */

#ifndef __XSET_H__
#define __XSET_H__

#include <X11/Xlib.h>
#include "econfig.h"

EDELIB_NAMESPACE {

/**
 * \class XSet
 * \brief A class for setting some X11 options.
 *
 * This class contains options that allows setting
 * a minimal set of options found in 'xset' command (run <em>man xset</em>
 * to see about them).
 *
 * It will try to open default display (0:0), or one if parameter is
 * specified. If fails, all further methods will be ignored.
 */

class XSet {
	private:
		Display* display;

	public:
		/**
		 * Creates object, with openning default X11 display (0:0)
		 */
		XSet();

		/**
		 * Creates object, openning display as given parameter
		 * \param d is display name
		 */
		XSet(const char* d);

		/**
		 * Clean data, closing display
		 */
		~XSet();

		/**
		 * Set options for mouse
		 */
		void set_mouse(int accel, int thresh);

		/**
		 * Set options for builtin computer bell (not speakers)
		 */
		void set_bell(int volume, int pitch, int duration);

		/**
		 * Set options for keyboard
		 */
		void set_keyboard(int repeat, int clicks);

		/**
		 * Play some sound via computer bell. Usefull for testing parameters given with set_bell()
		 */
		void test_bell(void);

		/**
		 * Blank screen. Usefull for testing parameters given with set_blank()
		 */
		void test_blank(void);
};

}

#endif
