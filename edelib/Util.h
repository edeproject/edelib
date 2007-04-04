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

EDELIB_NAMESPACE {

/*!\addtogroup functions
 * @{ */
/** 
 * Get default directory where should be stored user specific
 * configuration files. Place is determined according to the
 * XDG Base Directory Specification.
 *
 * \return filled String with path or empty String if fails
 * */
EDELIB_API String user_config_dir(void);

/** 
 * Get default directory where should be stored user specific
 * data files like icons. Place is determined according to the
 * XDG Base Directory Specification.
 *
 * \return filled String with path or empty String if fails
 * */
EDELIB_API String user_data_dir(void);

/** 
 * Get default directory where should be stored application specific
 * cached data. Place is determined according to the
 * XDG Base Directory Specification.
 *
 * \return filled String with path or empty String if fails
 * */
EDELIB_API String user_cache_dir(void);

/*! @} */
}
#endif
