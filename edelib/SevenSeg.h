/*
 * $Id$
 *
 * s7 segment number widget
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

#ifndef __EDELIB_SEVENSEG_H__
#define __EDELIB_SEVENSEG_H__

#include "edelib-global.h"
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
class EDELIB_API SevenSeg : public Fl_Widget {
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
#endif
