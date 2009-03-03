/*
 * $Id$
 *
 * Locale functions
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

#ifndef __NLS_H__
#define __NLS_H__

#include "edelib-global.h"

#ifdef USE_NLS
	#include <libintl.h>
	#define _(s)  gettext(s)
#else
	#define textdomain(domain) ((const char*)domain)
	#define bindtextdomain(domain, dir) ((const char*)dir)
	#define bind_textdomain_codeset(domain, codeset) ((const char*)codeset)
	#define _(s)  (s)
#endif

#define N_(s) (s)

EDELIB_NS_BEGIN

/**
 * Forces current locale to be set to "C".
 *
 * The main intent for this function is to provide uniform data representation for some 
 * functions across locales. For example, <i>strtod</i> or <i>printf</i> family depends on current locale and 
 * if you want predictable behaviour or reading/writing across locales, the best way is to set "C" locale, call 
 * this functions and restore previous locale.
 *
 * nls_locale_to_c() will return information of current locale, which is allocated c-string. You should not
 * free it, use nls_locale_from_c(ret_value) instead, like:
 * \code
 *   char* loc = nls_locale_to_c();
 *   strtod(...)
 *   nls_locale_from_c(loc);
 * \endcode
 *
 * \return old locale. It can return NULL if NLS is disabled; nls_locale_from_c() will handle that too.
 */
EDELIB_API char* nls_locale_to_c(void);

/**
 * Restore locale set with nls_locale_to_c().
 * \param old is previous locale retrieved with nls_locale_to_c()
 */
EDELIB_API void nls_locale_from_c(char* old);

/**
 * A shorthand for textdomain() and bindtextdomain()
 *
 * This function can be called before application starts, like:
 * \code
 *   int main()
 *   {
 *      // LOCALEDIR should be set somewhere
 *      // and advice is to use autoconf for that
 *      nls_support_init("my_killer_app", LOCALEDIR);
 *
 *      // app init, starting and etc.
 *   }
 * \endcode
 *
 * \todo  what should set_textdomain_dir() and set_textdomain() return when NLS is disabled?
 *
 * \param appname is a set of translatable messages, coresponding to the target application's executable
 * \param dir the base directory for appname
 */
EDELIB_API void nls_support_init(const char* appname, const char* dir);

EDELIB_NS_END
#endif // __NLS_H__
