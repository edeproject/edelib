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

/** 
 * Check if directory exists 
 */ 
EDELIB_API bool dir_exists(const char* name);

/** 
 * Check if directory is readable 
 */
EDELIB_API bool dir_readable(const char* name);

/** 
 * Check if directory is writeable 
 */
EDELIB_API bool dir_writeable(const char* name);

/**
 * Creates directory calling system's mkdir() with apropriate
 * permission (default is 0777).
 *
 * This function will fail if given path exists (directory, file, link, etc.)
 * Return true if succeded or false if failed.
 */
EDELIB_API bool dir_create(const char* name, int perm = 0777);

/**
 * Remove given path (must be directory) calling system's rmdir().
 * Directory <b>must</b> be empty or operation will fail.
 * Return true if succeded or false if failed.
 */
EDELIB_API bool dir_remove(const char* name);

/**
 * Remove given path recursively (must be a directory). If parameter <em>all</em>
 * is set to true (default is true), it will remove all files and directories under
 * <em>name</em> parameter (including given <em>name</em>). If set to true, only
 * files will be removed, leaving directory structure intact.
 *
 * Parameter <em>progress</em> is function externaly defined, which can be used to
 * track file/directory deletition. Can be used like:
 * \code
 *   void myprogress(const char* name) {
 *      printf("deleting %s\n", name);
 *   }
 *
 *   dir_remove_rec(some_path, true, myprogress)
 * \endcode
 *
 * If function fails to delete underlaying directories/files, it will call on_fail() given
 * function (otherwise will skip them).
 * 
 * \note Not implemented yet
 */
EDELIB_API bool dir_remove_rec(const char* name, bool all = true, 
		void (*progress)(const char* name) = NULL, 
		void (*on_fail)(const char* name) = NULL);

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
 *   vector<String> dlist;
 *   dir_list(path, dlist);
 *   dlist.empty() == true;
 * \endcode
 *
 * since is much faster.
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
 * Returns platform specific separator to delimit directories in path 
 */
EDELIB_API const char* dir_separator(void);

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
