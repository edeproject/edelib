/*
 * $Id$
 *
 * Message dialogs
 * Part of edelib.
 * Copyright (c) 2005-2007 EDE Authors.
 *
 * This program is licenced under terms of the 
 * GNU General Public Licence version 2 or newer.
 * See COPYING for details.
 */

#ifndef __ASK_H__
#define __ASK_H__

#include "econfig.h"

EDELIB_NAMESPACE {

/**
 * Displays a printf-style message in a pop-up box with an "Close" button, waits 
 * for the user to hit the button. The message will wrap to fit the window, or 
 * may be many lines by putting '\\n' characters into it.
 */
EDELIB_API void message(const char* fmt, ...);

/** Same as message() */
EDELIB_API void alert(const char* fmt, ...);

/**
 * Displays a printf-style message in a pop-up box with "Yes" and "No" buttons and
 * waits for the user to hit the button. If user hits "Yes", returned will be 1, or
 * 0 if user hits "No".
 */
EDELIB_API int ask(const char* fmt, ...);

/**
 * Pops up a window displaying a string, lets the user edit it, and return the new value. 
 * The cancel button returns NULL.
 */
EDELIB_API const char *input(const char *label, const char *deflt = 0, ...);

/** Same as input(), except input data are hidden with the stars */
EDELIB_API const char *password(const char *label, const char *deflt = 0, ...);

}

#endif
