#include <stdio.h>
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Shared_Image.H>
#include <edelib/IconChooser.h>
#include <edelib/Window.h>
#include <edelib/IconLoader.h>
#include <edelib/String.h>
#include <edelib/FileTest.h>

EDELIB_NS_USING(String)
EDELIB_NS_USING_AS(Window, EWindow)
EDELIB_NS_USING(file_test)
EDELIB_NS_USING(icon_chooser)
EDELIB_NS_USING(FILE_TEST_IS_DIR)

Fl_Box* image_box;

void close_cb(Fl_Widget*, void* w) {
	Fl_Window* ww = (Fl_Window*)w;
	ww->hide();
}

void run_cb(Fl_Widget*, void*) {
	/* const char* dd = "/opt/kde/share/icons/default.kde/48x48/apps/"; */
	const char* dd = "/home/sanel/.icons/edeneu/16x16/apps/";
	if(!file_test(dd, FILE_TEST_IS_DIR))
		printf("Directory %s does not exists, continuing...\n", dd);

	String r = icon_chooser(dd);

	if(!r.empty()) {
		image_box->image(Fl_Shared_Image::get(r.c_str()));
		image_box->redraw();
	}
}

int main() {
	EWindow* win = new EWindow(290, 180, "Icon chooser test");
	win->begin();
	Fl_Box* ibox = image_box = new Fl_Box(80, 10, 135, 110);
	ibox->box(FL_THIN_DOWN_BOX);
	Fl_Button* run = new Fl_Button(95, 145, 90, 25, "&Run");
	run->callback(run_cb);
	Fl_Button* close = new Fl_Button(190, 145, 90, 25, "&Close");
	close->callback(close_cb, win);
	win->end();
	win->show();
	return  Fl::run();
}
