#include <stdio.h>
#include <string.h>
#include <edelib/Theme.h>

EDELIB_NS_USE

void help(void) {
	puts("Usage: edelib-theme-script [OPTIONS] <file>");
	puts("Options:");
	puts("   -h   --help   display this help\n");
	puts("Load theme interpreter. If <file> was given, interpret it");
}

int main(int argc, char** argv) {
	if(argc != 1 && argc != 2) {
		help();
		return 1;
	}

	if(argc == 1) {
		Theme t;
		t.prompt();
		return 0;
	}

	if(argc == 2 && ((strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "--help") == 0))) {
		help();
		return 1;
	} else {
		Theme t;
		const char *f = argv[1];
		if(!t.load(f)) {
			printf("Unable to load %s\n", f);
			return 1;
		}
	}

	return 0;
}
