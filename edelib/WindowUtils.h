/*
 * $Id: WindowXid.h 2839 2009-09-28 11:36:20Z karijes $
 *
 * Window utils
 * Copyright (c) 2009-2011 edelib authors
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

#ifndef __EDELIB_WINDOWUTILS_H__
#define __EDELIB_WINDOWUTILS_H__

#include "edelib-global.h"

class Fl_Window;

EDELIB_NS_BEGIN

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
void window_xid_create(Fl_Window* win, void (*before_map_func)(Fl_Window*) = NULL, int background_pixel = -1);

/**
 * \ingroup widgets
 *
 * Center window on screen. It will take current window width and height and, including screen size, calculate correct
 * position and set window on it. The best way is to call this function before window <em>show()</em>.
 */
void window_center_on_screen(Fl_Window* win);

EDELIB_NS_END
#endif
