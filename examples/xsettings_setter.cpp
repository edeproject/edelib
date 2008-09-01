#include <edelib/XSettingsClient.h>
#include <edelib/Color.h>

#include <FL/Fl.h>
#include <FL/x.h>
#include <FL/Fl_Window.h>
#include <FL/Fl_Button.h>
#include <FL/Fl_Check_Button.h>
#include <FL/Fl_Input.h>
#include <FL/Fl_Color_Chooser.h>

#include <stdio.h>

EDELIB_NS_USE;

Fl_Button* button1;
Fl_Button* button2;
Fl_Check_Button* check_button;
Fl_Input* input_field;
XSettingsClient* xs_client;

void button1_cb(Fl_Widget*, void*) {
	unsigned char r, g, b;
	if(!fl_color_chooser("Choose color for background", r, g, b))
		return;
	button1->color(color_rgb_to_fltk(r, g, b));
	xs_client->set("Test/Background", r, g, b, 0);
	xs_client->manager_notify();
}

void button2_cb(Fl_Widget*, void*) {
	unsigned char r, g, b;
	if(!fl_color_chooser("Choose color for foreground", r, g, b))
		return;
	button2->color(color_rgb_to_fltk(r, g, b));
	xs_client->set("Test/Foreground", r, g, b, 0);
	xs_client->manager_notify();
}

void check_cb(Fl_Widget*, void*) {
	xs_client->set("Test/CheckValue", check_button->value());
	xs_client->manager_notify();
}

void type_cb(Fl_Widget*, void*) {
	xs_client->set("Test/Text", input_field->value());
	xs_client->manager_notify();
}

int main(int argc, char **argv) {
	// FIXME: fix this in XSettingsClient
	fl_open_display();

	XSettingsClient cl;
	if(!cl.init(fl_display, fl_screen)) {
		puts("Can't run XSETTINGS client");
		return 1;
	}

	xs_client = &cl;

	Fl_Window* win = new Fl_Window(225, 145, "XSETTINGS setter");
	win->begin();
		button1 = new Fl_Button(10, 30, 95, 25, "background");
		button1->align(FL_ALIGN_TOP_LEFT);
		button1->callback(button1_cb);

		button2 = new Fl_Button(120, 30, 95, 25, "foreground");
		button2->align(FL_ALIGN_TOP_LEFT);
		button2->callback(button2_cb);

		check_button = new Fl_Check_Button(10, 62, 205, 25, "Check in another window");
		check_button->down_box(FL_DOWN_BOX);
		check_button->callback(check_cb);

		input_field = new Fl_Input(10, 111, 205, 24, "Type some text:");
		input_field->align(FL_ALIGN_TOP_LEFT);
		input_field->callback(type_cb);
		input_field->when(FL_WHEN_CHANGED);
	win->end();
	win->show();
	return Fl::run();
}
