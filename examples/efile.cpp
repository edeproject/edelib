/*
 * Similar to file command but use XDG 
 * mime to deduce types
 */

#include <edelib/MimeType.h>
#include <stdio.h>
#include <string.h>

EDELIB_NS_USE;

int main(int argc, char** argv) {
	if(argc < 2) {
		puts("Usage: efile [FILES...]");
		puts("Determine file type of [FILES...]");
		return 1;
	}

	int mlen = 0;
	for(int i = 1, tl = 0; i < argc; i++) {
		tl = strlen(argv[i]);
		if(tl > mlen)
			mlen = tl;
	}


	MimeType mt;
	for(int i = 1; i < argc; i++) {
		if(!mt.set(argv[i])) {
			printf("Can't fetch type for %s, skipping...\n", argv[i]);
			continue;
		}

		//printf("%s: %30s (%s)\n", argv[i], mt.comment().c_str(), mt.type().c_str());
		printf("%s:", argv[i]);
		printf("%*s ", mlen - strlen(argv[i]), "");
		printf("%s (%s)\n", mt.comment().c_str(), mt.type().c_str());
	}

	return 0;
}
