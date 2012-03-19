#include <stdio.h>
#include <string.h>
#include <edelib/Version.h>
#include <edelib/Scheme.h>

static void help(void) {
	puts("Usage: edelib-script [OPTIONS] <file>");
	puts("Options:");
	puts("   -h   --help          display this help\n");
	puts("   -t   --theme  <file> load file as theme\n");
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

	if(argc == 2 && ((strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "--help") == 0))) {
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

	printf("edelib script %s. Type \"(quit)\" to exit.", EDELIB_VERSION);
	edelib_scheme_load_file(sc, stdin);

	edelib_scheme_deinit(sc);
	return 0;
}
