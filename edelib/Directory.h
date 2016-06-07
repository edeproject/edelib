/*
 * $Id$
 *
 * Directory handlers
 * Copyright (c) 2005-2007 edelib authors
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __EDELIB_DIRECTORY_H__
#define __EDELIB_DIRECTORY_H__

#include "String.h"
#include "List.h"

/**
 * \ingroup macros
 * \def E_DIR_SEPARATOR
 *
 * Platfom specific directory separator as single character
 */
#define E_DIR_SEPARATOR '/'

/**
 * \ingroup macros
 * \def E_DIR_SEPARATOR_STR
 *
 * Platfom specific directory separator as static string
 */
#define E_DIR_SEPARATOR_STR "/"

EDELIB_NS_BEGIN

/** 
 * Check if directory exists 
 * \deprecated with file_test()
 */ 
EDELIB_API bool dir_exists(const char* name) EDELIB_DEPRECATED;

/** 
 * Check if directory is readable 
 * \deprecated with file_test()
 */
EDELIB_API bool dir_readable(const char* name) EDELIB_DEPRECATED;

/** 
 * Check if directory is writeable 
 * \deprecated with file_test()
 */
EDELIB_API bool dir_writeable(const char* name) EDELIB_DEPRECATED;

/**
 * Creates directory calling system's mkdir() with apropriate
 * permission (default is 0777).
 *
 * This function will fail if given path exists (directory, file, link, etc.)
 * Return true if succeded or false if failed.
 */
EDELIB_API bool dir_create(const char* name, int perm = 0777);

/**
 * Creates directory if does not exists and create intermedaite parents
 * if needed too.
 *
 * Return true if succeded or false if failed.
 */
EDELIB_API bool dir_create_with_parents(const char* name, int perm = 0777);

/**
 * Remove given path (must be directory) calling system's rmdir().
 * Directory <b>must</b> be empty or operation will fail.
 * Return true if succeded or false if failed.
 */
EDELIB_API bool dir_remove(const char* name);

/**
 * Rename given directory name. If name to be renamed to exists (file, directory, link and etc.)
 * it will fail. Directory to be renamed <b>must</b> be empty.
 */
EDELIB_API bool dir_rename(const char* from, const char* to);

/**
 * Return true if given directory is empty. If path is pointing to anything
 * else than directory, it will return false. Use dir_exists() to check is path really a directory.
 *
 * This function is alternative to this code:
 * \code
 *   list<String> dlist;
 *   dir_list(path, dlist);
 *   dlist.empty() == true;
 * \endcode
 *
 * but is much faster.
 */
EDELIB_API bool dir_empty(const char* name);

/**
 * Returns user home directory or empty String if fails 
 */
EDELIB_API String dir_home(void);

/** 
 * Returns current directory or empty String if fails 
 */
EDELIB_API String dir_current(void);

/**
 * List content of given directory. If directory is accessible, given
 * parameter will be filled with the content.
 *
 * dir_list() can accept "." which will resolve to the current directory, but
 * will <b>not</b> accept ".." (directory up), nor will resolve chainings on 
 * it (like "../../../").
 *
 * list content will be cleared before items are added.
 *
 * \return true if target directory reading went fine; otherwise false
 * \param dir target directory
 * \param lst is a content of directory
 * \param full_path if set will append directory name to the each entry
 * \param show_hidden if set will show hidden files
 * \param show_dots if set will show <em>.</em> and <em>..</em> directories
 */
EDELIB_API bool dir_list(const char* dir, list<String>& lst, bool full_path = false, bool show_hidden = false,
		bool show_dots = false);

EDELIB_NS_END
#endif

