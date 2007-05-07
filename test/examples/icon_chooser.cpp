#include <fltk/Window.h>
#include <fltk/InvisibleBox.h>
#include <fltk/Button.h>
#include <fltk/SharedImage.h>
#include <edelib/IconChooser.h>
#include <edelib/String.h>
#include <edelib/Directory.h>
#include <fltk/run.h>
#include <stdio.h>

fltk::InvisibleBox* image_box;

void close_cb(fltk::Widget*, void* w)
{
	fltk::Window* ww = (fltk::Window*)w;
	ww->hide();
}

void run_cb(fltk::Widget*, void*) 
{
	//const char* dd = "/opt/kde/share/icons/default.kde/48x48/apps/";
	const char* dd = "/";
	if(!edelib::dir_exists(dd))
		printf("Directory %s does not exists, continuing...\n", dd);

	edelib::String r = edelib::icon_chooser(dd);

	if(!r.empty()) 
	{
		image_box->image(fltk::SharedImage::get(r.c_str()));
		image_box->redraw();
	}
}

int main() 
{
	fltk::Window* win = new fltk::Window(290, 180, "Icon chooser test");
	win->begin();
	fltk::InvisibleBox* ibox = image_box = new fltk::InvisibleBox(80, 10, 135, 110);
	ibox->box(fltk::THIN_DOWN_BOX);
	fltk::Button* run = new fltk::Button(95, 145, 90, 25, "&Run");
	run->callback(run_cb);
	fltk::Button* close = new fltk::Button(190, 145, 90, 25, "&Close");
	close->callback(close_cb, win);
	win->end();
	win->show();
	return  fltk::run();
}
