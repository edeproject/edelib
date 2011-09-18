#include <stdio.h>
#include <edelib/Theme.h>

EDELIB_NS_USE

void help(void) {
	puts("Usage: edelib-theme-script [file]");
	puts("Load [file] as theme script and interpret it");
}

int main(int argc, char** argv) {
	if(argc != 2) {
		help();
		return 1;
	}

	Theme t;
	const char *f = argv[1];

	if(!t.load(f)) 
		printf("Unable to load %s\n", f);

	return 0;
}
