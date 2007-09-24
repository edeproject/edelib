/*
 * $Id$
 *
 * Message dialog
 * Part of edelib.
 * Copyright (c) 2005-2007 EDE Authors.
 *
 * This program is licensed under terms of the 
 * GNU General Public License version 2 or newer.
 * See COPYING for details.
 */

#include <edelib/MessageBox.h>
#include <edelib/IconTheme.h>
#include <edelib/Nls.h>

#include <FL/Fl.h>
#include <FL/fl_draw.h>
#include <FL/Fl_Return_Button.h>
#include <FL/Fl_Shared_Image.h>
#include <FL/Fl_Pixmap.h>

#include <stdlib.h>
#include <stdarg.h>           //
#include <stdio.h>            //
#include "icons/warning.xpm"  // used by message(), ask()...
#include "icons/info.xpm"     //

#define MAX_TXT_W 365 // Max text width before dialog is started to be resized
#define MIN_TXT_W 225 // Min text width; used by Fl_Input mostly
#define WIN_W 310 
#define WIN_H 110

EDELIB_NS_BEGIN

MessageBox::MessageBox(MessageBoxType t) : Fl_Window(WIN_W, WIN_H),
	img(0), txt(0), inpt(0), pix(0), mbt(t), nbuttons(0) {
	begin();
		gr = new Fl_Group(0, 0, w(), h());
		init();
	end();
}

MessageBox::~MessageBox() { 
	delete pix;
}

void MessageBox::init(void) {
	gr->begin();
	img = new Fl_Box(10, 10, 60, 55);

	if(mbt == MSGBOX_PLAIN) {
		txt = new Fl_Box(75, 10, MIN_TXT_W, img->h());
	} else {
		txt = new Fl_Box(75, 10, MIN_TXT_W, 25);
		inpt = new Fl_Input(75, 40, MIN_TXT_W, 25);

		if(mbt == MSGBOX_INPUT_SECRET)
			inpt->type(FL_SECRET_INPUT);
	}

	txt->align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT | FL_ALIGN_WRAP);

	b_start = w() - 10;
	gr->end();
}

void MessageBox::clear(MessageBoxType t) {
	img = 0; txt = 0; inpt = 0; nbuttons = 0;
	b_start = 0;
	mbt = t;

	gr->clear();

	size(WIN_W, WIN_H);
	gr->size(w(), h());

	init();
}

void MessageBox::add_button(Fl_Button* b, MessageBoxButtonType bt) {
	if(nbuttons >= MSGBOX_MAX_BUTTONS)
		return;
	if(!b)
		return;

	int W = 0, H = 0;

	fl_font(b->labelfont(), b->labelsize());
	fl_measure(b->label(), W, H);

	if(W > 90) {
		W += 10; // some spaces between label an button edges
		if(bt == MSGBOX_BUTTON_RETURN)
			W += 30; // increase again since Fl_Return_Button have that stupid marker
		else
			W += 10; // ordinary button
		b_start -= W;
	} else {
		W = 90;
		b_start -= W;
	}

	b->resize(b_start, 75, W, 25);
	b_start -= 5; // space between buttons

	gr->add(b);
	buttons[nbuttons] = b;
	nbuttons++;
}

void MessageBox::add_button(const char* l, MessageBoxButtonType bt, Fl_Callback cb, void* param) {
	if(nbuttons >= MSGBOX_MAX_BUTTONS)
		return;

	Fl_Button* bb;

	// button will resized anyway
	if(bt == MSGBOX_BUTTON_RETURN)
		bb = new Fl_Return_Button(1, 1, 1, 1);
	else
		bb = new Fl_Button(1, 1, 1, 1);

	bb->label(l);

	if(cb)
		bb->callback(cb, param);

	add_button(bb, bt);
}

void MessageBox::fix_sizes(void) { 
	int W = MAX_TXT_W, H = 0, step = 0;
	int xdiff = 0, ydiff = 0;

	// This means that last added button goes beyond X=10 (in negative).
	if(b_start < 10)
		step = -b_start + 5; // 5 is addition as diff from edge

	fl_font(txt->labelfont(), txt->labelsize());
	fl_measure(txt->label(), W, H);

	if(W > txt->w())
		xdiff = W - txt->w();
	else
		W = txt->w(); // use MIN_TXT_W

	if(H > img->h()) {
		ydiff = H - img->h();
		if(inpt)
			ydiff += 30; // uh...
	} else {
		// use size from img
		if(!inpt)
			H = img->h();
		else
			H = txt->h(); // or default size, so it can nice lay with inpt
	}

	if(inpt)
		inpt->resize(inpt->x(), inpt->y() + ydiff, W + step, inpt->h());

	txt->size(W + step, H);

	// force group to not resize any children or will mess everything up
	gr->resizable(0);

	gr->size(w() + xdiff + step, h() + ydiff);
	size(w() + xdiff + step, h() + ydiff);

	for(int i = 0; i < nbuttons; i++)
		buttons[i]->position(buttons[i]->x() + xdiff + step, buttons[i]->y() + ydiff);

}

void MessageBox::run_plain(bool center) {
	fix_sizes();

	if(center) {
		int X, Y, W, H;
		Fl::screen_xywh(X, Y, W, H);
		position(W/2 - w()/2, H/2 - h()/2);
	}

	if(!shown())
		show();

	while(shown())
		Fl::wait();
}

int MessageBox::run(bool center) {
	if(!nbuttons) {
		run_plain();
		return -1;
	}
		
	fix_sizes();

	if(center) {
		int X, Y, W, H;
		Fl::screen_xywh(X, Y, W, H);
		position(W/2 - w()/2, H/2 - h()/2);
	}

	show();

	Fl_Widget* widget;
	int ret = -1;
	while(1) {
		widget = Fl::readqueue();
		if(!widget) {
			Fl::wait();
		} else if(widget == this) {
			ret = -1;
			goto dialog_end;
		} else {
			for(int i = 0; i < nbuttons; i++) {
				if(buttons[i] == widget) {
					ret = i;
					goto dialog_end;
				}
			}
		}
	}

dialog_end:
	hide();
	return ret;
}

void MessageBox::set_text(const char* t) {
	txt->copy_label(t);
}

bool MessageBox::set_icon(const char* path) {
	fl_register_images();

	Fl_Image* i = Fl_Shared_Image::get(path);
	if(!i)
		return false;

	img->image(i);
	return true;
}

bool MessageBox::set_theme_icon(const char* name) {
	if(!IconTheme::inited())
		return false;

	String p = IconTheme::get(name, ICON_SIZE_MEDIUM);
	if(p.empty())
		return false;

	return set_icon(p.c_str());
}

void MessageBox::set_xpm_icon(const char* const* arr) {
	delete pix;

	pix = new Fl_Pixmap(arr);
	img->image(pix);
}

const char* MessageBox::get_input(void) {
	if(!inpt)
		return NULL;
	return inpt->value();
}

void MessageBox::set_input(const char* txt) {
	if(inpt)
		inpt->value(txt);
}

void MessageBox::focus_button(int b) {
	if(nbuttons && b < nbuttons)
		buttons[b]->take_focus();
}


#define BUFF_LEN 1024
static char internal_buff[BUFF_LEN];
static char internal_ret_buff[BUFF_LEN];

static char msg_icon[BUFF_LEN]    = {0};
static char alert_icon[BUFF_LEN]  = {0};
static char ask_icon[BUFF_LEN]    = {0};
static char input_icon[BUFF_LEN]  = {0};
static char passwd_icon[BUFF_LEN] = {0};

#define DO_COPY(src, dest) \
	if(src) { \
		strncpy(dest, src, BUFF_LEN); \
		dest[BUFF_LEN - 1] = 0; \
	}

void themed_dialog_icons(const char* msg, const char* alert, const char* ask, const char* input, const char* password) {
	DO_COPY(msg, msg_icon)
	DO_COPY(alert, alert_icon)
	DO_COPY(ask, ask_icon)
	DO_COPY(input, input_icon)
	DO_COPY(password, passwd_icon)
}

void clear_dialog_icons(void) {
	msg_icon[0]   = 0;
	alert_icon[0] = 0;
	ask_icon[0]   = 0;
	input_icon[0] = 0;
	passwd_icon [0] = 0;
}

void message(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(internal_buff, BUFF_LEN, fmt, ap);
	va_end(ap);

	MessageBox mb;
	mb.set_text(internal_buff);
	mb.add_button(_("&Close"));

	if(msg_icon[0] != 0)
		mb.set_theme_icon(msg_icon);
	else
		mb.set_xpm_icon(info_xpm);

	mb.run();
}

void alert(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(internal_buff, BUFF_LEN, fmt, ap);
	va_end(ap);

	MessageBox mb;
	mb.set_text(internal_buff);
	mb.add_button(_("&Close"));

	if(alert_icon[0] != 0)
		mb.set_theme_icon(alert_icon);
	else
		mb.set_xpm_icon(warning_xpm);

	//mb.set_modal();
	mb.run();
}

int ask(const char *fmt, ...) {
	char buffer[BUFF_LEN];
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(buffer, BUFF_LEN - 1, fmt, ap);
	va_end(ap);

	MessageBox mb;
	mb.set_text(buffer);
	mb.add_button(_("&No"));
	mb.add_button(_("&Yes"));

	if(ask_icon[0] != 0)
		mb.set_theme_icon(ask_icon);
	// FIXME: set_xpm_icon()

	mb.set_modal();
	int ret = mb.run();
	return (ret > 0 ? ret : 0);
}

const char* input(const char *fmt, const char *deflt, ...) {
	va_list ap;
	va_start(ap, deflt);
	vsnprintf(internal_buff, BUFF_LEN, fmt, ap);
	va_end(ap);

	MessageBox mb(MSGBOX_INPUT);
	mb.set_text(internal_buff);

	if(deflt)
		mb.set_input(deflt);

	mb.add_button(_("&Cancel"));
	mb.add_button(_("&OK"));

	if(input_icon[0] != 0)
		mb.set_theme_icon(input_icon);
	// FIXME: set_xpm_icon()
	
	//mb.set_modal();
	int ret = mb.run();

	if(ret <= 0)
		return NULL;
	if(!mb.get_input())
		return NULL;

	strncpy(internal_ret_buff, mb.get_input(), BUFF_LEN);
	internal_ret_buff[BUFF_LEN-1] = 0;

	return internal_ret_buff;
}

const char* password(const char *fmt, const char *deflt, ...) {
	va_list ap;
	va_start(ap, deflt);
	vsnprintf(internal_buff, BUFF_LEN, fmt, ap);
	va_end(ap);

	MessageBox mb(MSGBOX_INPUT_SECRET);
	mb.set_text(internal_buff);

	if(deflt)
		mb.set_input(deflt);

	mb.add_button(_("&Cancel"));
	mb.add_button(_("&OK"));

	if(passwd_icon[0] != 0)
		mb.set_theme_icon(passwd_icon);
	// FIXME: set_xpm_icon()

	//mb.set_modal();
	int ret = mb.run();

	if(ret <= 0)
		return NULL;
	if(!mb.get_input())
		return NULL;

	strncpy(internal_ret_buff, mb.get_input(), BUFF_LEN);
	internal_ret_buff[BUFF_LEN-1] = 0;

	return internal_ret_buff;
}

// FLTK compatibility
void (*fl_message)(const char* fmt, ...) = message;
void (*fl_alert)(const char* fmt, ...)   = alert;
int  (*fl_ask)(const char* fmt, ...)     = ask;
const char* (*fl_input)(const char* fmt, const char* deflt, ...)    = input;
const char* (*fl_password)(const char* fmt, const char* deflt, ...) = password;

EDELIB_NS_END

