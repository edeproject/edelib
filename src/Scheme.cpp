/*
 * $Id$
 *
 * Scheme interpeter
 * Copyright (c) 2005-2012 edelib authors
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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <edelib/Missing.h>
#include <edelib/Scheme.h>
#include <edelib/Version.h>
#include <edelib/Directory.h>
#include <edelib/Debug.h>

scheme *edelib_scheme_init(void) {
	scheme *s;
	pointer sym;

	s = scheme_init_new();
	sym = edelib_scheme_mk_symbol(s, "*edelib-dir-separator*");
	edelib_scheme_define(s, EDELIB_SCHEME_GLOBAL_ENV(s), sym, edelib_scheme_mk_string(s, E_DIR_SEPARATOR_STR));

	sym = edelib_scheme_mk_symbol(s, "*edelib-version*");
	edelib_scheme_define(s, EDELIB_SCHEME_GLOBAL_ENV(s), sym, edelib_scheme_mk_string(s, EDELIB_VERSION));

	/* load init files; it is a list of files separated with ':' */
	char *paths = getenv("EDELIB_SCHEME_INIT");
	if(paths) {
		char *tmp = edelib_strndup(paths, 256);
		FILE *fd  = NULL;

		for(char *p = strtok(tmp, ":"); p; p = strtok(NULL, ":")) {
			fd = fopen(p, "r");

			if(!fd) {
				E_WARNING(E_STRLOC ": unable to open '%s'. Skipping...\n", p);
				continue;
			}

			edelib_scheme_load_file(s, fd);
			fclose(fd);
			fd = NULL;
		}

		free(tmp);
	} else {
		E_WARNING(E_STRLOC ": EDELIB_SCHEME_INIT wasn't set. Interpreter will load without init files\n");
	}

	return s;
}
