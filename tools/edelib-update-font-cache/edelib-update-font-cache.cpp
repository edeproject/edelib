#include <stdio.h>
#include <string.h>
#include <edelib/FontCache.h>

EDELIB_NS_USE

#define CHECK_ARGV(argv, pshort, plong) ((strcmp(argv, pshort) == 0) || (strcmp(argv, plong) == 0))

static void help(void) {
	puts("Usage: edelib-update-font-cache [OPTIONS]");
	puts("Cache all fonts readable from X server for FLTK/edelib applications");
	puts("Options:");
	puts("   -h   --help   display this help");
	puts("   -l   --list   list all available fonts");
	puts("   -c   --count  show count of cached fonts");
}

static void on_font(const char *n, FontInfo *f, void *data) {
	printf("%s (%i sizes)\n", n, f->nsizes);
}

static int list_fonts(void) {
	FontCache f;
	if(!f.load()) {
		puts("Unable to load database! Try to run this program without parameters first to cache fonts");
		return 1;
	}

	f.for_each_font_sorted(on_font);
	return 0;
}

static int count_fonts(void) {
	FontCache f;
	if(!f.load()) {
		puts("Unable to load database! Try to run this program without parameters first to cache fonts");
		return 1;
	}

	printf("%i\n", f.count());
	return 0;
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
		if(CHECK_ARGV(argv[1], "-h", "--help")) {
			help();
			return 1;
		} else if(CHECK_ARGV(argv[1], "-l", "--list")) {
			return list_fonts();
		} else if(CHECK_ARGV(argv[1], "-c", "--count")) {
			return count_fonts();
		} else {
			puts("Wrong option. Run '-h' to see options");
			return 1;
		}
	}

done:
	return 0;
}
