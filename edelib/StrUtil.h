/*
 * $Id$
 *
 * Basic functions for C string handling
 * Part of edelib.
 * Copyright (c) 2005-2007 EDE Authors.
 *
 * This program is licenced under terms of the 
 * GNU General Public Licence version 2 or newer.
 * See COPYING for details.
 */

#ifndef __STRUTIL_H__
#define __STRUTIL_H__

#include "econfig.h"

EDELIB_NAMESPACE {

//!\defgroup functions Functions

/*!\addtogroup functions
 * C-string like functions that operates directly on buffer.
 * \note Buffer <b>must</b> be writeable, so you can't just
 * do:
 *  \code
 *  str_trim(" foo ");
 *  \endcode
 * @{ */
/** removes leading spaces */
EDELIB_API char* str_trimleft(char* str);

/** removes trailing spaces */
EDELIB_API char* str_trimright(char* str);

/** removes leading and trailing spaces */
EDELIB_API char* str_trim(char* str);

/** makes all characters lower */
EDELIB_API unsigned char* str_tolower(unsigned char* str);

/** makes all characters upper */
EDELIB_API unsigned char* str_toupper(unsigned char* str);

/** check if string ends with given test string */
EDELIB_API bool str_ends(const char* str, const char* test);
/*! @} */
}
#endif
