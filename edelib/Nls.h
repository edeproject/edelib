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

const char* set_textdomain(const char* domain);
const char* set_textdomain_dir(const char* domain, const char* dir);
void init_locale_support(const char* appname, const char* dir);

}
#endif
