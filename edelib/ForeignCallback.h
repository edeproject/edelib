/*
 * $Id$
 *
 * Foreign callbacks
 * Copyright (c) 2005-2009 edelib authors
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

#ifndef __EDELIB_FOREIGNCALLBACK_H__
#define __EDELIB_FOREIGNCALLBACK_H__

#include "edelib-global.h"

class Fl_Window;

EDELIB_NS_BEGIN

/**
 * \defgroup foreigncallback Foreign callbacks
 */

/**
 * \ingroup foreigncallback
 *
 * foreign_callback_add(), foreign_callback_remove() and foreign_callback_call() functions provides
 * a simple and lightweight mechanism for calling functions in other applications. For example, one application
 * can change some settings and store it in known file and request other application to update itself by reading it.
 *
 * These functions can be used as alternative to D-BUS calls in simple cases (and reduce dependency on D-BUS libraries),
 * but should not be used where D-BUS is appropriate, like sending additional data to remote functions.
 *
 * Each foreign callback is associated with <i>id</i>, a some string value used as calling method from external application.
 * That value can be anything, ranging from application name to some random character; the only requirement is
 * that external caller has the same string value. Here is one example that will print some text in receiver file:
 * \code
 *   // receiver.cpp
 *   void receive_cb(Fl_Window *win) {
 *     puts("Function called");
 *   }
 *
 *   int main() {
 *     Fl_Window *win = ...
 *     // initialization code and etc.
 *     foreign_callback_add(win, receive_cb, "my-cool-app");
 *     return Fl::run();
 *   }
 * \endcode
 *
 * \code
 *   // sender.cpp
 *   int main() {
 *     foreign_callback_call("my-cool-app");
 *     return 0;
 *   }
 * \endcode
 *
 * \todo Add some better examples
 *
 * Because foreign callback functions shares static internal data and the data is not locked, the best
 * way to initialize foreign callbacks is before windows are shown in single application; with this no X messages
 * will be sent and you are assured nothing will be called unexpectedly.
 *
 * foreign_callback_add() and foreign_callback_call() will open X display if not so.
 *
 * Because these functions depends on FLTK and X, they are part of libedelib_gui library.
 */
void foreign_callback_add(Fl_Window *win, void (*cb)(Fl_Window *win), const char *id);

/**
 * \ingroup foreigncallback
 * Remove already registered callback if exists.
 */
void foreign_callback_remove(void (*cb)(Fl_Window *win));

/**
 * \ingroup foreigncallback
 * Call foreign function by registered string id.
 */
void foreign_callback_call(const char *id);

EDELIB_NS_END
#endif

