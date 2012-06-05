/*
 * $Id: String.h 2839 2009-09-28 11:36:20Z karijes $
 *
 * Tooltip support for menus
 * Copyright (c) 2012 edelib authors
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

#ifndef __EDELIB_STRING_H__
#define __EDELIB_STRING_H__

#include "edelib-global.h"
#include <FL/Fl_Widget.H>

EDELIB_NS_BEGIN

/**
 * \class MenuTooltip
 * \brief Tooltip support inside menus
 *
 * This class allows menu items (see MenuItem) to have tooltips. Unfortunately, Fl_Tooltip can't be
 * used as it does not allow displaying menu window alongside the tooltip window.
 *
 * MenuTooltip is used internally by MenuItem class, so you should not use it directly; use Fl_Tooltip instead. Also
 * MenuTooltip will use the same preferences as Fl_Tooltip, like background color, delay time, color and etc.
 */
class MenuTooltip {
private:
	static Fl_Widget *widget_;
	static void exit_(Fl_Widget*);
public:
	/** Behaves the same as Fl_Tooltip::enter_area() member. */
	static void enter_area(Fl_Widget *w, int X, int Y, int W, int H, const char *tip);
	/** Same as Fl_Tooltip::current(Fl_Widget*) */
	static void current(Fl_Widget*);
	/** Same as Fl_Tooltip::current() */
	static Fl_Widget *current(void) { return widget_; }
};

EDELIB_NS_END
#endif
