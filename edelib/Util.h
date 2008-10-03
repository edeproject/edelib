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
 * This function will construct a path, separating each item with E_DIR_SEPARATOR separator.
 * Each parameter will be separated with the one separator, except the case when one of the 
 * parameters contains multiple path elements separated with multiple separators.
 *
 * Also, the number of leading separators in the first parameter and the number of trailing
 * separators in the last parameter will be preserved.
 *
 * For example:
 * \code
 *   build_filename("home", "foo", "baz") == "home/foo/baz";
 *   build_filename("myplace/dir", "myfile") == "myplace/dir/myfile";
 *
 *   // keep heading separators
 *   build_filename("/home", "foo", "baz") == "/home/foo/baz";
 *   build_filename("///home", "foo", "baz") == "///home/foo/baz";
 *
 *   // keep trailing separators
 *   build_filename("/home/foo", "foofile", "baz/") == "/home/foo/foofile/baz/";
 *   build_filename("/home/foo", "foofile", "baz///") == "/home/foo/foofile/baz///";
 *
 *   // first parameter have multiple elements, it will be preserved as is
 *   build_filename("/home///foo", "//foofile///", "/baz") == "/home///foo/foofile/baz";
 *
 *   // correct trailing data, since does not match to separator
 *   build_filename("///home////foo", "//foofile///", "/baz//") == "/home/foo/foofile/baz";
 *
 *   // if given only one parameter, heading and trailing number of separator will be preserved
 *   build_filename("///home///") == "///home///";
 * \endcode
 *
 * \return constructed filename path
 * \param p1 first parameter
 * \param p2 optional second; if is NULL (default), it is ignored
 * \param p3 optional third; if is NULL (default), it is ignored
 */
EDELIB_API String build_filename(const char* p1, const char* p2 = NULL, const char* p3 = NULL);

EDELIB_NS_END
#endif // __UTIL_H__
