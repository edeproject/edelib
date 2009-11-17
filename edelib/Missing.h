/*
 * $Id$
 *
 * Provides the missing functions
 * Copyright (c) 2005-2007 edelib authors
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __EDELIB_MISSING_H__
#define __EDELIB_MISSING_H__

#include "edelib-global.h"

#ifdef __cplusplus
extern "C" {
#endif

/* include dirent intentionally so we can emulate it if missing */
#include <dirent.h>

/**
 * \defgroup missing Missing function
 */

/**
 * \ingroup missing
 * Implementation of setenv() function. Older libc implementations on various
 * UNIX-es does not provide it.
 */
EDELIB_API int edelib_setenv(const char* name, const char* value, int overwrite);

/**
 * \ingroup missing
 * Implementation of unsetenv() function.
 */
EDELIB_API int edelib_unsetenv(const char* name);

/**
 * Implementation of strnlen() function.
 */
EDELIB_API unsigned int edelib_strnlen(const char* str, unsigned int maxlen);

/**
 * \ingroup missing
 * Implementation of strndup() function.
 */
EDELIB_API char* edelib_strndup(const char* str, unsigned int maxlen);

/**
 * \ingroup missing
 * Implementation of strlcpy() function. This function operates similar
 * to the strncpy() except it will always terminate string with '\\0' character,
 * unless sz == 0. Returns strlen(src).
 */
EDELIB_API unsigned long edelib_strlcpy(char* dst, const char* src, unsigned long sz);

/**
 * \ingroup missing
 * Implementation of strlcat() function. This function operates similar
 * to the strncat() except it will always terminate string with '\\0' character,
 * unless sz == 0. At most sz-1 characters will be copied. Returns strlen(dst) + strlen(src).
 */
EDELIB_API unsigned long edelib_strlcat(char* dst, const char* src, unsigned long sz);

/**
 * \ingroup missing
 * scandir() implementation.
 */
EDELIB_API int edelib_scandir(const char* dir, struct dirent*** namelist, 
		int (*filter)(const struct dirent* name),
		int (*compar)(struct dirent** n1, struct dirent** n2));

/**
 * \ingroup missing
 * alphasort() implementation; a function that comes with scandir().
 */
EDELIB_API int edelib_alphasort(struct dirent **n1, struct dirent **n2);

/**
 * \ingroup missing
 * daemon() implementation.
 */
EDELIB_API int edelib_daemon(int nochdir, int noclose);

#ifdef __cplusplus
}
#endif

#endif
