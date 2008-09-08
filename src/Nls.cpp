/*
 * $Id$
 *
 * Locale functions
 * Part of edelib.
 * Copyright (c) 2005-2008 EDE Authors.
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

char* nls_locale_to_c(void) {
	char* old = NULL;
#ifdef USE_NLS
	char* loc = setlocale(LC_ALL, "");
	old = loc ? strdup(loc) : strdup("C");
	setlocale(LC_ALL, "C");
#endif
	return old;
}

void nls_locale_from_c(char* old) {
#ifdef USE_NLS
	setlocale(LC_ALL, old);
	free(old);
#endif
}

void nls_support_init(const char* appname, const char* dir) {
#ifdef USE_NLS
	setlocale(LC_MESSAGES, "");
	bindtextdomain(appname, dir);
	textdomain(appname);
#endif
}

EDELIB_NS_END
