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

#include <stdio.h>
#include <string.h>
#include <edelib/Version.h>
#include <edelib/Scheme.h>

#define CHECK_ARGV(argv, pshort, plong) ((strcmp(argv, pshort) == 0) || (strcmp(argv, plong) == 0))

static void help(void) {
	puts("Usage: edelib-script [OPTIONS] <file>");
	puts("Options:");
	puts("   -h   --help          display this help\n");
	puts("Load interpreter. If <file> was given, interpret it");
}

int main(int argc, char** argv) {
	bool prompt = false;
	
	if(argc != 1 && argc != 2) {
		help();
		return 1;
	}

	if(argc == 1)
		prompt = true;

	if(argc == 2 && CHECK_ARGV(argv[1], "-h", "--help")) {
		help();
		return 1;
	}
	
	scheme *sc = edelib_scheme_init();
	if(!sc) {
		puts("Unable to load interpreter");
		return 1;
	}
	
	edelib_scheme_set_input_port_file(sc, stdin);
	edelib_scheme_set_output_port_file(sc, stdout);

	if(prompt) {
		printf("edelib script %s. Type \"(quit)\" to exit.", EDELIB_VERSION);
		edelib_scheme_load_file(sc, stdin);
	} else {
		FILE *fd = fopen(argv[1], "r");
		if(!fd) {
			printf("Unable to load '%s' as scheme source\n", argv[1]);
			return 1;
		}

		edelib_scheme_load_file(sc, fd);
		fclose(fd);
	}

	edelib_scheme_deinit(sc);
	return 0;
}
