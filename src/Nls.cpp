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

#include <edelib/Nls.h>

#ifdef USE_NLS
	#include <locale.h>
#endif

EDELIB_NS_BEGIN

const char* set_textdomain(const char* domain) {
#ifdef USE_NLS
	return textdomain(domain);
#else
	return "";
#endif
}

const char* set_textdomain_dir(const char* domain, const char* dir) {
#ifdef USE_NLS
	return bindtextdomain(domain, dir);
#else
	return "";
#endif
}

void init_locale_support(const char* appname, const char* dir) {
#ifdef USE_NLS
	setlocale(LC_MESSAGES, "");
	set_textdomain_dir(appname, dir);
	set_textdomain(appname);
#endif
}

EDELIB_NS_END
