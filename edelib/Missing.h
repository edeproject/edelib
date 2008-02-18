/*
 * $Id$
 *
 * Provide missing functions
 * Part of edelib.
 * Copyright (c) 2005-2007 EDE Authors.
 *
 * This program is licensed under terms of the 
 * GNU General Public Licence version 2 or newer.
 * See COPYING for details.
 */

#ifndef __MISSING_H__
#define __MISSING_H__

#include "econfig.h"

/*
 * These function have C linkage and are not part of
 * the main namespace.
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Implementation of setenv() function. Older libc implementations on various
 * UNIX-es does not provide it.
 */
EDELIB_API int edelib_setenv(const char* name, const char* value, int overwrite);

/**
 * Implementation of unsetenv() function.
 */
EDELIB_API int edelib_unsetenv(const char* name);

/**
 * Implementation of strnlen() function.
 */
EDELIB_API unsigned int edelib_strnlen(const char* str, unsigned int maxlen);

/**
 * Implementation of strndup() function.
 */
EDELIB_API char* edelib_strndup(const char* str, unsigned int maxlen);

/**
 * Implementation of strlcpy() function. This function operates similar
 * to the strncpy() except it will always terminate string with '\\0' character,
 * unless sz == 0. Returns strlen(src).
 */
EDELIB_API unsigned long edelib_strlcpy(char* dst, const char* src, unsigned long sz);

/**
 * Implementation of strlcat() function. This function operates similar
 * to the strncat() except it will always terminate string with '\\0' character,
 * unless sz == 0. At most sz-1 characters will be copied. Returns strlen(dst) + strlen(src).
 */
EDELIB_API unsigned long edelib_strlcat(char* dst, const char* src, unsigned long sz);

#ifdef __cplusplus
}
#endif

#endif
