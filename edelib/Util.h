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

EDELIB_NAMESPACE {

/*!\addtogroup functions
 * @{ */
/** 
 * Get default directory where should be stored user specific
 * configuration files. Place is determined according to the
 * XDG Base Directory Specification.
 *
 * \return C-string pointing to internal buffer, which overwritten with each new call.
 *
 * \note user_config_dir(), user_data_dir() and user_cache_dir() share the same
 * buffer and each call to any of them will overwrite it. You \b should \b copy
 * returned data to own buffer.
 *
 * \todo These functions needs more testing.
 * */
EDELIB_API const char* user_config_dir(void);

/** 
 * Get default directory where should be stored user specific
 * data files like icons. Place is determined according to the
 * XDG Base Directory Specification.
 *
 * \return C-string pointing to internal buffer, which overwritten with each new call.
 * */
EDELIB_API const char* user_data_dir(void);

/** 
 * Get default directory where should be stored application specific
 * cached data. Place is determined according to the
 * XDG Base Directory Specification.
 *
 * \return C-string pointing to internal buffer, which overwritten with each new call.
 * */
EDELIB_API const char* user_cache_dir(void);

/*! @} */
}
#endif
