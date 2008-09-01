#include <FL/Fl_Window.h>
#include <FL/Fl_Box.h>
#include <FL/Fl_Button.h>
#include <FL/Fl_Shared_Image.h>
#include <edelib/IconChooser.h>
#include <edelib/String.h>
#include <edelib/Directory.h>
#include <FL/Fl.h>
#include <stdio.h>

Fl_Box* image_box;

void close_cb(Fl_Widget*, void* w) {
	Fl_Window* ww = (Fl_Window*)w;
	ww->hide();
}

void run_cb(Fl_Widget*, void*) {
	const char* dd = "/opt/kde/share/icons/default.kde/48x48/apps/";
	//const char* dd = "/home/sanel/.icons/edeneu/16x16/apps/";
	//const char* dd = "/";
	if(!edelib::dir_exists(dd))
		printf("Directory %s does not exists, continuing...\n", dd);

	edelib::String r = edelib::icon_chooser(dd);

	if(!r.empty()) {
		image_box->image(Fl_Shared_Image::get(r.c_str()));
		image_box->redraw();
	}
}

int main() {
	Fl_Window* win = new Fl_Window(290, 180, "Icon chooser test");
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
