#include <stdio.h>
#include <string.h>
#include <edelib/FontCache.h>

EDELIB_NS_USE

void help(void) {
	puts("Usage: edelib-update-font-cache [OPTIONS]");
	puts("Options:");
	puts("   -h   --help   display this help\n");
	puts("Cache all fonts readable from X server for FLTK/edelib applications");
}

int main(int argc, char **argv) {
	if(argc != 1 && argc != 2) {
		help();
		return 1;
	}

	if(argc == 1) {
		int n = FontCache::init_db();
		if(n < 0) {
			puts("Unable to cache fonts. Check permissions in $XDG_CACHE_DIR folder or disk/memory free space");
			return 1;
		}
		printf("Cached %i fonts\n", n);
		goto done;
	}

	if(argc == 2) {
		if((strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "--help") == 0)) {
			help();
			return 1;
		} else {
			puts("Wrong option. Run '-h' to see options");
			return 1;
		}
	}

done:
	return 0;
}
