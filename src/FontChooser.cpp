/*
 * $Id$
 *
 * Font chooser
 * Part of edelib.
 * Copyright (c) 2005-2007 EDE Authors.
 *
 * This program is licensed under terms of the 
 * GNU General Public License version 2 or newer.
 * See COPYING for details.
 */

#include <edelib/FontChooser.h>

#include <FL/Fl.H>
#include <FL/Fl_Window.h>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Button.H>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _(s) s

#define DEFAULT_SIZE 12
#define DEFAULT_SIZE_STR "12"

EDELIB_NS_BEGIN

Fl_Window* win;
Fl_Hold_Browser* font_browser;
Fl_Hold_Browser* size_browser;
Fl_Int_Input* size_input;
Fl_Input* preview_input;

int numfaces = 0;
int* numsizes = 0;
int** sizes = 0;
int pickedsize = DEFAULT_SIZE;
int pickedfont = 0;

static void cancel_cb(Fl_Widget*, void*) {
	pickedfont = -1;
	pickedsize = -1;
	win->hide();
}

static void ok_cb(Fl_Widget*, void*) {
	win->hide();
}

static void size_cb(Fl_Widget*, long) {
	int i = size_browser->value();
	if(!i)
		return;
	const char* c = size_browser->text(i);
	while(*c < '0' || *c > '9')
		c++;
	pickedsize = atoi(c);
	size_input->value(c);

	preview_input->textsize(pickedsize);
	preview_input->redraw();
}

static void size_input_cb(Fl_Widget*, void*) {
	const char* typed = size_input->value();
	if(!typed)
		return;

	// only scan inside [1, 64)
	int typed_int = atoi(typed);
	if(typed_int < 1 || typed_int >= 64)
		return;

	pickedsize = typed_int;

	for(int i = 1; i <= size_browser->size(); i++) {
		if(strcmp(typed, size_browser->text(i)) == 0) {
			size_browser->value(i);
			break;
		}
	}

	preview_input->textsize(pickedsize);
	preview_input->redraw();
}

static void font_cb(Fl_Widget*, long) {
	int fn = font_browser->value();
	if(!fn)
		return;
	fn--;

	pickedfont = fn;

	preview_input->textfont((Fl_Font)fn);
	preview_input->textsize(pickedsize);
	preview_input->redraw();

	size_browser->clear();

	int n = numsizes[fn];
	int* s = sizes[fn];
	char buff[20];

	if(!n) {
		return;
	} else if(s[0] == 0) {
		// many sizes
		for(int i = 1; i < 64; i++) {
			sprintf(buff, "%d", i);
			size_browser->add(buff);
		}
		size_browser->value(pickedsize);
	} else {
		// some sizes
		int tmp = 0;
		for(int i = 0; i < n; i++) {
			if(s[i] <= pickedsize)
				tmp = i;
			sprintf(buff, "%d", s[i]);
			size_browser->add(buff);
		}
		size_browser->value(tmp + 1);
	}
}

void load_fonts(const char* family) {
	const char* name;
	int ftype;
	numfaces = Fl::set_fonts(family);
	sizes = new int*[numfaces];
	numsizes = new int[numfaces];

	// so 'delete sizes[i]' don't crash dialog
	for(int i = 0; i < numfaces; i++)
		sizes[i] = 0;

	int* s;
	int fs;
	for(int i = 0; i < numfaces; i++) {
		name = Fl::get_font_name((Fl_Font)i, &ftype);
		font_browser->add(name);

		fs = Fl::get_font_sizes((Fl_Font)i, s);
		numsizes[i] = fs;

		if(fs) {
			sizes[i] = new int[fs];
			for(int j = 0; j < fs; j++)
				sizes[i][j] = s[j];
		}
	}
}

int font_chooser(const char* name, const char* family, int& retsize, const char* default_name, int default_size) {
	win = new Fl_Window(450, 320, name);
	win->begin();
		font_browser = new Fl_Hold_Browser(10, 25, 335, 170, _("Font:"));
		font_browser->align(FL_ALIGN_TOP_LEFT);
		font_browser->callback(font_cb);

		size_input = new Fl_Int_Input(350, 25, 90, 25, _("Size:"));
		size_input->align(FL_ALIGN_TOP_LEFT);
		size_input->callback(size_input_cb);
		size_input->when(FL_WHEN_CHANGED);

		size_browser = new Fl_Hold_Browser(350, 55, 90, 140);
		size_browser->align(FL_ALIGN_TOP_LEFT);
		size_browser->callback(size_cb);

		preview_input = new Fl_Input(10, 220, 430, 55, _("Preview:"));
		preview_input->align(FL_ALIGN_TOP_LEFT);
		preview_input->value("abcdefgh ABCDEFGH");

		Fl_Button* ok_button = new Fl_Button(255, 285, 90, 25, _("&OK"));
		ok_button->callback(ok_cb);

		Fl_Button* cancel_button = new Fl_Button(350, 285, 90, 25, _("&Cancel"));
		cancel_button->callback(cancel_cb);
	win->end();

	load_fonts(family);

	font_browser->value(1);
	size_browser->value(DEFAULT_SIZE);
	size_input->value(DEFAULT_SIZE_STR);

	// try to set default values (TODO: scanning again !)
	if(default_name) {
		for(int i = 1; i <= font_browser->size(); i++) {
			if(strcmp(default_name, font_browser->text(i)) == 0) {
				font_browser->value(i);
				break;
			}
		}
	}

	if(default_size > 0) {
		char num[128];
		sprintf(num, "%d", default_size);
		for(int i = 1; i < size_browser->size(); i++) {
			if(strcmp(num, size_browser->text(i)) == 0) {
				size_browser->value(i);
				size_input->value(num);

				break;
			}
		}
	}

	// now update widgets
	font_cb(font_browser, 0);

	win->show();
	while(win->shown())
		Fl::wait();

	delete [] numsizes;
	for(int i = 0; i < numfaces; i++)
		delete sizes[i];
	delete [] sizes;

	if(retsize)
		retsize = pickedsize;

	return pickedfont;
}

EDELIB_NS_END
