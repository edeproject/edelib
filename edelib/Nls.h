/*
 * $Id$
 *
 * Locale functions
 * Part of edelib.
 * Copyright (c) 2005-2007 EDE Authors.
 *
 * This program is licenced under terms of the 
 * GNU General Public Licence version 2 or newer.
 * See COPYING for details.
 */

#ifndef __NLS_H__
#define __NLS_H__

#include "econfig.h"

#ifdef USE_NLS
	#include <libintl.h>
	#define _(s)  gettext(s)
#else
	#define _(s)  (s)
#endif

#define N_(s) (s)

EDELIB_NAMESPACE {
/**
 * Sets or retrieves the current message domain.
 *
 * A message domain is a set of translatable messages. Usually, every software package has its own
 * message domain. The domain name is used to determine the message catalog where a  translation  is  looked
 * up; it must be a non-empty string.
 *
 * \return the current message domain if domain is not NULL; returned string must not be modified or freed
 * \param domain is current message domain; if domain is NULL, the function returns the current message domain
 */
const char* set_textdomain(const char* domain);

/**
 * Sets the base directory of the hierarchy containing message catalogs for a given message domain.
 *
 * Message catalogs will be expected at the pathnames dirname/locale/category/domainname.mo, where locale is
 * a locale name and category is a locale facet such as LC_MESSAGES.
 *
 * set_textdomain() and set_textdomain_dir() should be used in form:
 * \code
 *   setlocale(LC_MESSAGES, "");
 *   set_textdomain_dir(appname, dir);
 *   set_textdomain(appname);
 * \endcode
 *
 * \return the current base directory for domain; returned string must not be modified or freed
 * \param domain same as for set_textdomain() and must not be NULL
 * \param dir the base directory for message catalogs belonging to domain; it dir is NULL, the function 
 * returns the previously set base directory for domain 
 */
const char* set_textdomain_dir(const char* domain, const char* dir);

/**
 * A shorthand for set_textdomain() and set_textdomain_dir()
 *
 * This function can be called before application starts, like:
 * \code
 *   int main()
 *   {
 *      // LOCALEDIR should be set somewhere
 *      // and advice is to use autoconf for that
 *      init_locale_support("my_killer_app", LOCALEDIR);
 *
 *      // app init, starting and etc.
 *   }
 * \endcode
 *
 * \param appname is a set of translatable messages, coresponding to the target application's executable
 * \param dir the base directory for appname
 */
void init_locale_support(const char* appname, const char* dir);

}
#endif
