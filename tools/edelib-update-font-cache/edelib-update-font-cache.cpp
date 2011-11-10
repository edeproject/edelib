#include <stdio.h>
#include <string.h>
#include <edelib/FontCache.h>

EDELIB_NS_USE

static void help(void) {
	puts("Usage: edelib-update-font-cache [OPTIONS]");
	puts("Options:");
	puts("   -h   --help   display this help");
	puts("   -l   --list   list all available fonts\n");
	puts("Cache all fonts readable from X server for FLTK/edelib applications");
}

static void on_font(const char *n, FontInfo *f, void *data) {
	printf("%s (%i sizes)\n", n, f->nsizes);
}

static void list_fonts(void) {
	FontCache f;
	if(!f.load()) {
		puts("Unable to load database! Try to run this program without parameters first to cache fonts");
		return;
	}

	f.for_each_font(on_font);
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
		} else if((strcmp(argv[1], "-l") == 0) || (strcmp(argv[1], "--list") == 0)) {
			list_fonts();
			return 0;
		} else {
			puts("Wrong option. Run '-h' to see options");
			return 1;
		}
	}

done:
	return 0;
}
