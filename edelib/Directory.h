/*
 * $Id$
 *
 * Directory handlers
 * Part of edelib.
 * Copyright (c) 2005-2007 EDE Authors.
 *
 * This program is licenced under terms of the 
 * GNU General Public Licence version 2 or newer.
 * See COPYING for details.
 */

#ifndef __DIRECTORY_H__
#define __DIRECTORY_H__

#include "econfig.h"
#include "String.h"

EDELIB_NAMESPACE {

/** Check if directory exists */
bool dir_exists(const char* name);

/** Check if directory is readable */
bool dir_readable(const char* name);

/** Check if directory is writeable */
bool dir_writeable(const char* name);

/** Returns user home directory or empty String if fails */
String dir_home(void);

/** Returns current directory or empty String if fails */
String dir_current(void);

/** Returns platform specific separator to delimit directories in path */
String dir_separator(void);

}

#endif
