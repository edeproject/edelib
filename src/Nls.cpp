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
