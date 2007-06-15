/*
 * $Id$
 *
 * Message dialogs
 * Part of edelib.
 * Copyright (c) 2005-2007 EDE Authors.
 *
 * This program is licenced under terms of the
 * GNU General Public Licence version 2 or newer.
 * See COPYING for details.
 */

#include <edelib/Ask.h>
#include <edelib/econfig.h>

#include <fltk/InvisibleBox.h>
#include <fltk/Group.h>
#include <fltk/Button.h>
#include <fltk/SharedImage.h>
#include <fltk/Input.h>
#include <fltk/Window.h>
#include <fltk/run.h>
#include <fltk/draw.h>

#include <stdarg.h>
#include <stdio.h>  // vsnprintf
#include <string.h> // strncpy

#define BUTTON_SPACING   5		// spacing between buttons
#define BUTTON_W         90		// button default width
#define WIDGET_H_SPACING 10		// horizontal spacing between two widgets
#define BUFF_SIZE        1024

EDELIB_NAMESPACE {

using namespace fltk;

// TODO: remove this to safer place;
char input_buff[BUFF_SIZE];

enum MessageDialogType {
	PLAIN = 0,
	INPUT
};

class MessageDialog : public Window {
private:
	MessageDialogType dtype;
	InvisibleBox* icon;
	InvisibleBox* text;
	Input*        text_input;
	Group* button_group;
	int ret_value;

public:
	MessageDialog(MessageDialogType t = PLAIN) : Window(365, 125, ""), dtype(t) {
		begin();
		icon = new InvisibleBox(10, 10, 70, 70);
		text = new InvisibleBox(90, 25, 265, 25);
		text->box(FLAT_BOX);
		text->align(ALIGN_TOP|ALIGN_LEFT|ALIGN_INSIDE|ALIGN_WRAP);

		if (dtype != PLAIN) {
			text_input = new Input(90, 55, 265, 25);
		}

		button_group = new Group(10, 90, 345, 25);
		end();
	}
	~MessageDialog() { }

	void set_image(const Symbol& s) {
		icon->image(s);
	}
	void set_text(const char* txt) {
		text->copy_label(txt);
	}

	void set_ret(int v) {
		ret_value = v;
	}
	int get_ret(void) {
		return ret_value;
	}

	void pick_input(void) {
		strncpy(input_buff, text_input->value(), BUFF_SIZE);
	}
	void set_input(const char* v) {
		text_input->value(v);
		strncpy(input_buff, v, BUFF_SIZE);
	}
	const char* get_input(void) const {
		return input_buff;
	}

	void show(void) {
		set_modal();
		exec();
	}
	void hide(void) {
		Window::hide();
	}

	void add_button(const char* lbl, Callback* cb) {
		// max 3 buttons
		if (button_group->children() == 3)
			return;

		Button* b;
		if (!button_group->children())
			b = new Button((button_group->w() - BUTTON_W), 0, 90, 25, lbl);
		else {
			Button* last = (Button*)button_group->child(button_group->children() - 1);
			b = new Button( (last->x() - BUTTON_W - BUTTON_SPACING), 0, 90, 25, lbl);
		}
		b->callback(cb, this);
		button_group->add(b);
	}

	void layout() {
		int tw = text->w();
		int th;
		// resize text as needed
		setfont(text->labelfont(), text->labelsize());
		measure(text->label(), tw, th, text->flags());

		if (th > text->h()) {
			int offset = th + (int)text->labelsize();
			text->h( offset );

			int boffset;
			if (dtype == PLAIN)
				boffset = text->y() + text->h() + WIDGET_H_SPACING;
			else {
				text_input->y( text->y() + text->h() + WIDGET_H_SPACING );
				boffset = text_input->y() + text_input->h() + WIDGET_H_SPACING;
			}
			//button_group->y( text->y() + text->h() + WIDGET_H_SPACING);
			button_group->y(boffset);
			h( button_group->y() + button_group->h() + WIDGET_H_SPACING);
		}
	}
};

void close_cb(Widget* w, void* data) {
	MessageDialog* win = (MessageDialog*)data;
	win->hide();
}

void ret0_val_cb(Widget* w, void* data) {
	MessageDialog* win = (MessageDialog*)data;
	win->set_ret(0);
	win->hide();
}

void ret1_val_cb(Widget* w, void* data) {
	MessageDialog* win = (MessageDialog*)data;
	win->set_ret(1);
	win->hide();
}

void ret_str_val_cb(Widget* w, void* data) {
	MessageDialog* win = (MessageDialog*)data;
	win->pick_input();
	win->hide();
}

void message(const char* fmt, ...) {
	char buffer[BUFF_SIZE];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, ap);
	va_end(ap);

	MessageDialog m;
	m.set_text(buffer);
	m.add_button("&Close", close_cb);
	m.show();
}

void alert(const char* fmt, ...) {
	char buffer[BUFF_SIZE];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, ap);
	va_end(ap);

	MessageDialog m;
	m.set_text(buffer);
	m.add_button("&Close", close_cb);
	m.show();
}

int ask(const char* fmt, ...) {
	char buffer[BUFF_SIZE];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, ap);
	va_end(ap);

	MessageDialog m;
	m.set_text(buffer);
	m.add_button("&No", ret0_val_cb);
	m.add_button("&Yes", ret1_val_cb);
	m.show();
	return m.get_ret();
}

const char* input(const char *label, const char *deflt, ...) {
	char buffer[BUFF_SIZE];
	va_list ap;
	va_start(ap, deflt);
	vsnprintf(buffer, sizeof(buffer), label, ap);
	va_end(ap);

	MessageDialog m(INPUT);
	m.set_text(buffer);

	if (deflt)
		m.set_input(deflt);

	m.add_button("&Close", ret_str_val_cb);
	m.add_button("&OK", ret_str_val_cb);
	m.show();
	return m.get_input();
}

}

