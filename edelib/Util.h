/*
 * $Id$
 *
 * Various functions
 * Part of edelib.
 * Copyright (c) 2005-2007 EDE Authors.
 *
 * This program is licenced under terms of the 
 * GNU General Public Licence version 2 or newer.
 * See COPYING for details.
 */

#ifndef __UTIL_H__
#define __UTIL_H__

#include "econfig.h"
#include "String.h"
#include "List.h"

EDELIB_NS_BEGIN

/** 
 * Get default directory where should be stored user specific
 * configuration files. Place is determined according to the
 * XDG Base Directory Specification.
 *
 * \return filled String with path or empty String if fails <b>without</b>
 * directory separator at the end
 * */
EDELIB_API String user_config_dir(void);

/** 
 * Get default directory where should be stored user specific
 * data files like icons. Place is determined according to the
 * XDG Base Directory Specification.
 *
 * \return filled String with path or empty String if fails <b>without</b>
 * directory separator at the end
 * */
EDELIB_API String user_data_dir(void);

/** 
 * Get default directory where should be stored application specific
 * cached data. Place is determined according to the
 * XDG Base Directory Specification.
 *
 * \return filled String with path or empty String if fails <b>without</b>
 * directory separator at the end
 * */
EDELIB_API String user_cache_dir(void);

/**
 * Get a list of directories where configuration files should be searched.
 * List is determined according to the XDG Base Directory Specification.
 *
 * \return a size of list
 * \param lst is where to put founded data
 */
EDELIB_API int system_config_dirs(list<String>& lst);

/**
 * Get a list of directories where data files should be searched.
 * List is determined according to the XDG Base Directory Specification.
 *
 * \return a size of list
 * \param lst is where to put founded data
 */
EDELIB_API int system_data_dirs(list<String>& lst);

/**
 * This function will create correct filename path, separating each item
 * with exact one separator. It will also correct possible given incorrect parameters.
 * Function will not touch trailing separator, if given.
 *
 * Sample:
 * \code
 *   build_filename("/", "/home/foo", "foofile", "baz") == "/home/foo/foofile/baz";
 *   build_filename("/", "myplace/dir", "myfile") == "myplace/dir/myfile";
 *   // corrections
 *   build_filename("/", "/home////foo", "//foofile///", "/baz//") == "/home/foo/foofile/baz";
 *   // correct trailing data, since does not match to separator
 *   build_filename("/", "///home////foo", "//foofile///", "/baz//") == "/home/foo/foofile/baz";
 *   // also correct first parameter
 *   build_filename("/", "/home/foo/////") == "/home/foo";
 * \endcode
 *
 * \return string with given parameters
 * \param separator is separator
 * \param p1 first parameter
 * \param p2 optional second; if is NULL (default), it is ignored
 * \param p3 optional third; if is NULL (default), it is ignored
 */
EDELIB_API String build_filename(const char* separator, const char* p1, const char* p2 = NULL, const char* p3 = NULL);

/**
 * This function behaves the same as build_filename(), except will always add ending separator, like:
 * \code
 *   build_dirname("/", "/home/foo") == "/home/foo/";
 *   build_dirname("/", "home","mydir") == "home/mydir/";
 * \endcode
 */
EDELIB_API String build_dirname(const char* separator, const char* p1, const char* p2 = NULL, const char* p3 = NULL);

EDELIB_NS_END
#endif // __UTIL_H__
