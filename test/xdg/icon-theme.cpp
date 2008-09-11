#include <stdio.h>
#include <stdlib.h>
#include <edelib/IconTheme.h>

EDELIB_NS_USE

static bool validate_icon_size(int sz) {
	if(sz == ICON_SIZE_TINY    ||
		sz == ICON_SIZE_SMALL  ||
		sz == ICON_SIZE_MEDIUM ||
		sz == ICON_SIZE_LARGE  ||
		sz == ICON_SIZE_HUGE   ||
		sz == ICON_SIZE_ENORMOUS)
	{
		return true;
	}

	return false;
}

int main(int argc, char** argv) {
	if(argc != 4) {
		printf("Usage: %s <icon-theme> <icon-name> <icon-size>\n", argv[0]);
		return 1;
	}

	const char* icon_theme = argv[1];
	const char* icon_name = argv[2];
	int icon_size = atoi(argv[3]);

	if(!validate_icon_size(icon_size)) {
		printf("Unsupported (%i) icon size. Try again\n", icon_size);
		return 1;
	}

	IconTheme::init(icon_theme);
	printf("%s\n", IconTheme::get(icon_name, (IconSizes)icon_size).c_str());
	IconTheme::shutdown();

	return 0;
}
