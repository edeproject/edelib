#include <edelib/XSettingsClient.h>
#include <edelib/Color.h>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/x.H>

#include <stdio.h>
#include <string.h>

EDELIB_NS_USE

XSettingsClient* xs_client;
Fl_Box* test_box = NULL;

void xsettings_cb(const char* name, XSettingsAction action, XSettingsSetting* setting, void* data) {
	char* action_str;
	switch(action) {
		case XSETTINGS_ACTION_NEW:
			action_str = "new";
			break;
		case XSETTINGS_ACTION_CHANGED:
			action_str = "changed";
			break;
		case XSETTINGS_ACTION_DELETED:
			printf("deleted %s\n", name);
			return;
		default:
			action_str = "unknown";
			break;
	}

	if(setting->type == XSETTINGS_TYPE_STRING) {
		if(strcmp(setting->name, "Test/Text") == 0)
			test_box->copy_label(setting->data.v_string);
		printf("Got name %s with action '%s' and value '%s' ", name, action_str, setting->data.v_string);
	} else if(setting->type == XSETTINGS_TYPE_INT) {
		if(strcmp(setting->name, "Test/CheckValue") == 0) {
			if(setting->data.v_int == 1)
				test_box->box(FL_BORDER_BOX);
			else
				test_box->box(FL_DOWN_BOX);
			test_box->redraw();
		}
		printf("Got name %s with action '%s' and value '%i' ", name, action_str, setting->data.v_int);
	} else if(setting->type == XSETTINGS_TYPE_COLOR) {
		if(strcmp(setting->name, "Test/Background") == 0) {
			test_box->color(color_rgb_to_fltk(setting->data.v_color.red,
						setting->data.v_color.green,
						setting->data.v_color.blue));
			test_box->redraw();
		}

		if(strcmp(setting->name, "Test/Foreground") == 0) {
			test_box->labelcolor(color_rgb_to_fltk(setting->data.v_color.red,
						setting->data.v_color.green,
						setting->data.v_color.blue));
			test_box->redraw();
		}
	
		printf("Got name %s with action '%s' and value '%i %i %i %i' ", name, action_str, 
				setting->data.v_color.red,
				setting->data.v_color.green,
				setting->data.v_color.blue,
				setting->data.v_color.green);
	} else
		printf("Got unknown (%i) action", action);

	printf(" serial %lu\n", setting->last_change_serial);
}

int xevent_handler(int e) {
	xs_client->process_xevent(fl_xevent);
	return 1;
}

int main() {
	Fl_Window* win = new Fl_Window(200, 130, "XSETTINGS client");
	win->begin();
		test_box = new Fl_Box(10, 9, 180, 112, "Waiting input from other window...");
		test_box->box(FL_DOWN_BOX);
		test_box->color(FL_BACKGROUND2_COLOR);
		test_box->align(FL_ALIGN_TOP_LEFT|FL_ALIGN_INSIDE|FL_ALIGN_WRAP);
    win->end();
	win->show();

	XSettingsClient cl;
	if(!cl.init(fl_display, fl_screen, xsettings_cb, NULL)) {
		puts("Can't run XSETTINGS client");
		return 1;
	}

	xs_client = &cl;

	Fl::add_handler(xevent_handler);

	return Fl::run();
}
