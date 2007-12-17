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
int edelib_setenv(const char* name, const char* value, int overwrite);

/**
 * Implementation of unsetenv() function.
 */
int edelib_unsetenv(const char* name);

/**
 * Implementation of strnlen() function.
 */
unsigned int edelib_strnlen(const char* str, unsigned int maxlen);

/**
 * Implementation of strndup() function.
 */
char* edelib_strndup(const char* str, unsigned int maxlen);

#ifdef __cplusplus
}
#endif

#endif
