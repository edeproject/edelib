/*
 * $Id$
 *
 * s7 segment number widget
 * Part of edelib.
 * Copyright (c) 2000-2007 EDE Authors.
 *
 * This program is licenced under terms of the
 * GNU General Public Licence version 2 or newer.
 * See COPYING for details.
 */

#ifndef __SEVENSEG_H__
#define __SEVENSEG_H__

#include "econfig.h"
#include <FL/Fl_Widget.H>

EDELIB_NS_BEGIN

/**
 * \ingroup widgets
 * \class SevenSeg
 * \brief Digital number widget
 *
 * SevenSeg is widget for displaying s7 digital numbers.
 *
 * \image html sevenseg.jpg
 *
 * Setting and retrieving a number is accomplished via value()
 * methods. It accepts 0-9 range of integers or '0'-'9' range
 * of characters. Any number above or below will be ignored, defaulting to it's bounds.
 */
class SevenSeg : public Fl_Widget {
	private:
		int digit;
		int segwidth;

		void draw_seg_a(int X, int Y, int W, int H);
		void draw_seg_b(int X, int Y, int W, int H);
		void draw_seg_c(int X, int Y, int W, int H);
		void draw_seg_d(int X, int Y, int W, int H);
		void draw_seg_e(int X, int Y, int W, int H);
		void draw_seg_f(int X, int Y, int W, int H);
		void draw_seg_g(int X, int Y, int W, int H);

	public:
		/**
		 * Constructor
		 */
		SevenSeg(int X, int Y, int W, int H);

		/**
		 * Empty destructor
		 */
		~SevenSeg();
#ifndef SKIP_DOCS
		virtual void draw(void);
#endif
		/**
		 * Set number to be displayed. Range is 0-9.
		 */
		void value(int v);

		/**
		 * Set character to be displayed. Range is '0'-'9'.
		 */
		void value(char c);

		/**
		 * Get current displayed number.
		 */
		int  value(void) { return digit; }

		/**
		 * Set width of bars. Range is 2-12. Below or above will
		 * be ignored, replacing with given bounds.
		 */
		void bar_width(int w);

		/**
		 * Get current width of bars.
		 */
		int  bar_width(void) { return segwidth; }
};

EDELIB_NS_END
#endif // __SEVENSEG_H__
