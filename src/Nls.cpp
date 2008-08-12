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
# include <locale.h>
# include <string.h>
# include <stdlib.h>
#endif

EDELIB_NS_BEGIN

char* set_locale_to_c(void) {
	char* old = NULL;
#ifdef USE_NLS
	char* loc = setlocale(LC_ALL, "");
	old = loc ? strdup(loc) : strdup("C");
	setlocale(LC_ALL, "C");
#endif
	return old;
}

void set_locale_from_c(char* old) {
#ifdef USE_NLS
	setlocale(LC_ALL, old);
	free(old);
#endif
}

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
