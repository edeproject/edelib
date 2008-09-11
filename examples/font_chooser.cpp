#include <edelib/FontChooser.h>

EDELIB_NS_USE

int main() {
	int r;
	font_chooser("Font chooser sample", "is8859-1", r);
	//font_chooser("Font chooser sample", "-*", r);
	return 0;
}
