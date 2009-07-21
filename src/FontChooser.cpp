/*
 * $Id$
 *
 * Font chooser
 * Copyright (c) 2005-2007 edelib authors
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>

#include <edelib/FontChooser.h>
#include <edelib/Nls.h>
#include <edelib/Debug.h>

#define DEFAULT_SIZE     12
#define DEFAULT_SIZE_STR "12"

#define FONT_REGULAR     0
#define FONT_BOLD        1
#define FONT_ITALIC      2
#define FONT_BOLD_ITALIC 3

EDELIB_NS_BEGIN

static Fl_Window* win;
static Fl_Hold_Browser* font_browser;
static Fl_Hold_Browser* size_browser;
static Fl_Hold_Browser* style_browser;
static Fl_Int_Input* size_input;
static Fl_Input* preview_input;

static int picked_size;
static int picked_font;
static int picked_style;

struct FontDetails {
	const char* name;   /* points to FLTK internal name (not allocated) */
	int* sizes;         /* allocated list of sizes */
	int num_sizes;      /* number of items in above list */
	int fltk_font;      /* FLTK font */
};

struct FontFamily {
	FontDetails* regular;
	FontDetails* bold;
	FontDetails* italic;
	FontDetails* bold_italic;
};

static FontFamily* font_family;
static int         font_family_size;

static FontDetails* curr_font_details;

static void cancel_cb(Fl_Widget*, void*) {
	picked_font = -1;
	picked_size = -1;
	picked_style = -1;
	win->hide();
}

static void ok_cb(Fl_Widget*, void*) {
	win->hide();
}

static void size_cb(Fl_Widget*, long) {
	int fs = size_browser->value();
	if(!fs) {
		/*
		 * Not clicked, or clicked on empty region; try to use previous font size
		 * and if not found in sizes of selected font, use it's first available size
		 */
		bool found = false;
		for(int i = 0; i < curr_font_details->num_sizes; i++) {
			if(curr_font_details->sizes[i] == picked_size) {
				found = true;
				break;
			}
		}

		if(found) {
			int val;
			int pos = 1;

			for(int i = 1; i <= size_browser->size(); i++) {
				val = atoi(size_browser->text(i));
				if(val == picked_size) {
					pos = i;
					break;
				}
			}

			size_browser->value(pos);
		} else {
			picked_size = curr_font_details->sizes[0];
			size_browser->value(1);
		}

		char num[6];
		sprintf(num, "%i", picked_size);
		size_input->value(num);
	} else {
		picked_size = atoi(size_browser->text(fs));
		size_input->value(size_browser->text(fs));
	}

	picked_font = curr_font_details->fltk_font;

	preview_input->textsize(picked_size);
	preview_input->textfont(picked_font);
	preview_input->redraw();
}

static void size_input_cb(Fl_Widget*, void*) {
	const char* typed = size_input->value();
	if(!typed)
		return;

	/* only scan inside [1, 64] */
	int typed_int = atoi(typed);
	if(typed_int < 1 || typed_int > 64)
		return;

	int val;
	for(int i = 1; i <= size_browser->size(); i++) {
		val = atoi(size_browser->text(i));
		/* found it; store it globaly and redraw */
		if(val == typed_int) {
			picked_size = val;
			size_browser->value(i);
			preview_input->textsize(picked_size);
			preview_input->redraw();
			return;
		}
	}
}

static void style_cb(Fl_Widget*, long) {
	int fs = style_browser->value();
	if(!fs) {
		style_browser->value(1);
		fs = 1;
	}

	const char* style_name = style_browser->text(fs);

	/* get what was selected in font browser */
	int sel_font = font_browser->value();
	sel_font--;

	E_ASSERT(sel_font <= font_family_size && "List item out of bounds");

	FontDetails* fd = NULL;

	if(strcmp(style_name, "Regular") == 0) {
		E_ASSERT(font_family[sel_font].regular != 0 && "Font marked as regular but not allocated");
		fd = font_family[sel_font].regular;
	} else if(strcmp(style_name, "Bold") == 0) {
		E_ASSERT(font_family[sel_font].bold != 0 && "Font marked as bold but not allocated");
		fd = font_family[sel_font].bold;
	} else if(strcmp(style_name, "Italic") == 0) {
		E_ASSERT(font_family[sel_font].italic != 0 && "Font marked as bold but not allocated");
		fd = font_family[sel_font].italic;
	} else if(strcmp(style_name, "Bold Italic") == 0) {
		E_ASSERT(font_family[sel_font].bold_italic != 0 && "Font marked as bold-italic but not allocated");
		fd = font_family[sel_font].bold_italic;
	} else {
		/* never reached */
		E_ASSERT(0 && "Unknown font name");
	}

	curr_font_details = fd;

	/* now fill size browser */
	size_browser->clear();

	char num[6];
	for(int i = 0; i < fd->num_sizes; i++) {
		/* FLTK for some fonts can set 0 size, skip it */
		if(fd->sizes[i] == 0)
			continue;

		sprintf(num, "%i", fd->sizes[i]);
		size_browser->add(num);
	}

	/* let size_cb find matching font size, select it and perform preview */
	size_cb(0, 0);
}

static void font_cb(Fl_Widget*, long) {
	int fn = font_browser->value();
	if(!fn) {
		font_browser->value(1);
		fn = 1;
	}

	/* Fl_Browser starts from 1 */
	fn--;

	E_ASSERT(fn <= font_family_size && "List item out of bounds");

	style_browser->clear();

	if(font_family[fn].regular)
		style_browser->add("Regular");
	if(font_family[fn].bold)
		style_browser->add("Bold");
	if(font_family[fn].italic)
		style_browser->add("Italic");
	if(font_family[fn].bold_italic)
		style_browser->add("Bold Italic");

	/* always select the first one */
	style_browser->value(1);

	/* let style_cb handle the rest */ 
	style_cb(0, 0);
}

static void clear_font_details(FontDetails* fd) {
	/* we can accept NULL (due not found regular/bold/... structs) so check this */
	if(!fd)
		return;

	delete [] fd->sizes;
	delete fd;
}

static void load_fonts(const char* family) {
	int nfonts = Fl::set_fonts(family);
	if(!nfonts)
		return;

	font_family = new FontFamily[nfonts];
	font_family_size = 0;

	for(int i = 0; i < nfonts; i++) {
		font_family[i].regular = 0;
		font_family[i].bold = 0;
		font_family[i].italic = 0;
		font_family[i].bold_italic = 0;
	}
	
	int ftype;
	int* s;
	int fs;
	const char* name;
	int* sizes;
	bool new_family_found;

	for(int i = 0; i < nfonts; i++) {
		name = Fl::get_font_name((Fl_Font)i, &ftype);
		fs = Fl::get_font_sizes((Fl_Font)i, s);

		if(!fs) {
			/* no sizes; skip this font */
			continue;
		} else if(s[0] == 0) {
			/* many sizes, use range (0, 64] then */
			fs = 64;
			sizes = new int[fs];
			for(int j = 0; j < fs; j++)
				sizes[j] = j + 1;
		} else {
			/* regular copy sizes */
			sizes = new int[fs];
			for(int j = 0; j < fs; j++)
				sizes[j] = s[j];
		}

		FontDetails* fd = new FontDetails;
		fd->name = name;
		fd->sizes = sizes;
		fd->num_sizes = fs;
		fd->fltk_font = i;

		new_family_found = false;

		/*
		 * The only way to locate a 'family' of fonts in FLTK's font list
		 * is to search between returned font types whose are 0. They are
		 * marked as regular. Idealy, FLTK will sort fonts in order 
		 * regular-bold-italic-bold_italic, but this will not always be true due
		 * bad comparison function inside FLTK and especially in case when
		 * XFT is used; sometimes FLTK will put regular type after bold, or italic.
		 *
		 * This is how is handled above cases here: if we found ftype to be 0,
		 * this is considered as start of new family. If not, check types on current
		 * family; if type already exists just skip it. This will probably miss some fonts...
		 */
		if(ftype == 0) {
			/* already have 'regular', but steped to another one, consider it as a new family */
			if(font_family[font_family_size].regular) {
				font_family_size++;
				new_family_found = true;
			}

			font_family[font_family_size].regular = fd;
		} else if(ftype == FL_BOLD) {
			if(font_family[font_family_size].bold) {
				E_DEBUG(E_STRLOC ": duplicate for: '%s' (previous was: '%s')\n",
						fd->name, font_family[font_family_size].bold->name);

				clear_font_details(fd);
			} else 
				font_family[font_family_size].bold = fd;

		} else if(ftype == FL_ITALIC) {
			if(font_family[font_family_size].italic) {
				E_DEBUG(E_STRLOC ": duplicate for: '%s' (previous was: '%s')\n",
						fd->name, font_family[font_family_size].italic->name);

				clear_font_details(fd);
			} else
				font_family[font_family_size].italic = fd;

		} else if(ftype == (FL_BOLD | FL_ITALIC)) {
			if(font_family[font_family_size].bold_italic) {
				E_DEBUG(E_STRLOC ": duplicate for: '%s' (previous was: '%s')\n",
						fd->name, font_family[font_family_size].bold_italic->name);

				clear_font_details(fd);
			} else
				font_family[font_family_size].bold_italic = fd;
		}

		/* sanity checks; should never happen */
		E_ASSERT(font_family_size < nfonts && "font_family_size bigger than the sum of all fonts!!!");

		/*
		 * Now add it to main font list (special case is when we starts
		 * first font is first family or first family will not be displayed)
		 */
		if(new_family_found || i == 0)
			font_browser->add(fd->name);
	}
}

int font_chooser(const char* name, const char* family, int& retsize, const char* default_name, int default_size) {
	picked_size = DEFAULT_SIZE;
	picked_font = 0;
	picked_style = 0;

	win = new Fl_Window(450, 320, name);
	win->size_range(450, 320);
	win->begin();
		/* invisible resizable box */
		Fl_Box* rbox = new Fl_Box(20, 56, 30, 24);
		win->resizable(rbox);

		font_browser = new Fl_Hold_Browser(10, 25, 170, 170, _("Font:"));
		font_browser->align(FL_ALIGN_TOP_LEFT);
		font_browser->callback(font_cb);

		style_browser = new Fl_Hold_Browser(185, 25, 160, 170, _("Style:"));
		style_browser->align(FL_ALIGN_TOP_LEFT);
		style_browser->callback(style_cb);

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
	
	/* now update the widgets */
	font_cb(0, 0);

	win->show();
	while(win->shown())
		Fl::wait();

	for(int i = 0; i <= font_family_size; i++) {
		clear_font_details(font_family[i].regular);
		clear_font_details(font_family[i].bold);
		clear_font_details(font_family[i].italic);
		clear_font_details(font_family[i].bold_italic);
	}

	if(retsize)
		retsize = picked_size;

	return picked_font;
}

EDELIB_NS_END
