#include <stdio.h>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.h>
#include <edelib/ThemeLoader.h>

EDELIB_NS_USE

static void change_cb(Fl_Widget*, void*) {
	if(!ThemeLoader::global()->load_with_path("theme-sample"))
		return;

	char buf[128];
	if(ThemeLoader::global()->theme()->get_item("ede", "sample", buf, sizeof(buf)))
		printf("evaluated => %s\n", buf);
}

int main() {
	Fl_Button *bb;
	Fl_Window *win = new Fl_Window(300, 300, "Theme demonstration");
	ThemeLoader::init();

	win->begin();
		bb = new Fl_Button(30, 30, 90, 25, "Click me");
		bb->callback(change_cb);
	win->end();
	win->show();
	Fl::run();

	ThemeLoader::shutdown();
	return 0;
}
