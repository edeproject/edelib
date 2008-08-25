#include <edelib/FontChooser.h>

using namespace edelib;

int main() {
	int r;
	font_chooser("Font chooser sample", "is8859-1", r);
	//font_chooser("Font chooser sample", "-*", r);
	return 0;
}
