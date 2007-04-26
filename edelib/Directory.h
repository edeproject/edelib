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
#include "Vector.h"

EDELIB_NAMESPACE {

/** Check if directory exists */ 
EDELIB_API bool dir_exists(const char* name);

/** Check if directory is readable */
EDELIB_API bool dir_readable(const char* name);

/** Check if directory is writeable */
EDELIB_API bool dir_writeable(const char* name);

/** Returns user home directory or empty String if fails */
EDELIB_API String dir_home(void);

/** Returns current directory or empty String if fails */
EDELIB_API String dir_current(void);

/** Returns platform specific separator to delimit directories in path */
EDELIB_API String dir_separator(void);

/**
 * List content of given directory. If directory is accessible, given
 * parameter will be filled with the content.
 *
 * dir_list() can accept "." which will resolve to the current directory, but
 * will <b>not</b> accept ".." (directory up), nor will resolve chainings on 
 * it (like "../../../").
 *
 * \return true if target directory reading went fine; otherwise false
 * \param dir target directory
 * \param lst is a content of directory
 * \param full_path if set will append directory name to the each entry
 * \param show_hidden if set will show hidden files
 */
EDELIB_API bool dir_list(const char* dir, vector<String>& lst, bool full_path = false, bool show_hidden = false);
}

#endif
