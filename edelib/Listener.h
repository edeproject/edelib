/*
 * $Id$
 *
 * Descriptor monitoring functions
 * Copyright (c) 2008 edelib authors
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

#ifndef __LISTENER_H__
#define __LISTENER_H__

#include "edelib-global.h"

EDELIB_NS_BEGIN

enum {
	LISTENER_READ   = 1, ///< callback is called when there is data to be read 
	LISTENER_WRITE  = 4, ///< callback is called when there is data to be written without blocking
	LISTENER_EXCEPT = 8  ///< callback is called when exception happens on descriptor
};

/**
 * listener_xxx are set of functions with ability to monitor file descriptors
 * (files, sockets, etc.). They are doing much the same work as FLTK's Fl::add_fd() and Fl::remove_fd()
 * with the Fl::wait() loop.
 *
 * They are mainly created to avoid linking with FLTK libraries when is not needed, eg. console
 * applications.
 *
 * listener_add_fd() will add file descriptor to listen to. Whem descriptor becomes ready, a <i>cb</i>
 * callback will be called.
 *
 * \param fd is descriptor to be monitored
 * \param when is bitfield (LISTENER_READ, LISTENER_WRITE and LISTENER_EXCEPT) to indicate when
 *        callback should be done
 * \param cb is callback to be called
 * \param arg is optional parameter passed to the callback
 */
EDELIB_API void listener_add_fd(int fd, int when, void(*cb)(int, void*), void* arg = 0);

/**
 * listener_add_fd() function with LISTENER_READ bit set.
 */
EDELIB_API inline void listener_add_fd(int fd, void(*cb)(int, void*), void* arg = 0) {
	listener_add_fd(fd, LISTENER_READ, cb, arg);
}

/**
 * Removes added descriptor.
 *
 * \param fd is descriptor to be removed
 * \param when are bits to be removed from <i>fd</i>.
 */
EDELIB_API void listener_remove_fd(int fd, int when);

/**
 * listener_remove_fd() function with LISTENER_READ bit set.
 */
EDELIB_API inline void listener_remove_fd(int fd) { 
	listener_remove_fd(fd, LISTENER_READ); 
}

/**
 * This function corresponds (in some parts) to the FLTK's Fl::wait(). In this case, it will wait until 
 * some changes happens on monitored descriptors and will return. It will also call given callbacks (via listener_add_fd()).
 *
 * listener_wait(), on other hand, is not replacement for Fl::wait(). It will not handle all things Fl::wait()
 * does (like refreshing windows, calling idle callbacks, etc.) and in case listener_xxx be used with
 * FLTK elements, listener_wait() must be called too.
 *
 * \return positive value if an event or fd happens before time elapsed. It is zero if nothing happens and negative
 *         is if error occurs, like signal
 * \param t is time to wait maximum seconds. It can return much sooner if something happens
 */
EDELIB_API double listener_wait(double t);

/**
 * The same as for listener_wait(time), except it will run forever, until something happens and will return
 */
EDELIB_API inline int listener_wait(void) { 
	// FLTK uses 1e20 for forever so I'm using it too
	return (int)listener_wait(1e20); 
}

EDELIB_NS_END

#endif
