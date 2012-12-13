/*
 * $Id$
 *
 * Font chooser
 * Copyright (c) 2005-2012 edelib authors
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
#include <edelib/FontCache.h>
#include <edelib/Nls.h>
#include <edelib/Debug.h>
#include <edelib/Missing.h>
#include <edelib/StrUtil.h>

#define DEFAULT_SIZE     12
#define DEFAULT_SIZE_STR "12"

EDELIB_NS_BEGIN

static Fl_Window *win;
static Fl_Hold_Browser *font_browser;
static Fl_Hold_Browser *size_browser;
static Fl_Hold_Browser *style_browser;
static Fl_Int_Input *size_input;
static Fl_Input *preview_input;
static FontCache *font_cache;
static FontInfo  *current_font;

static int picked_size;
static int picked_font;
static int picked_style;

static char face_buf[EDELIB_FONT_CACHE_FACE_LEN];

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
	E_RETURN_IF_FAIL(current_font != NULL);

	int fs = size_browser->value();
	if(!fs) {
		/*
		 * Not clicked, or clicked on empty region; try to use previous font size
		 * and if not found in sizes of selected font, use it's first available size
		 */
		bool found = false;
		for(int i = 0; i < current_font->nsizes; i++) {
			if(current_font->sizes[i] == picked_size) {
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
			picked_size = current_font->sizes[0];
			size_browser->value(1);
		}

		char num[6];
		snprintf(num, sizeof(num), "%i", picked_size);
		size_input->value(num);
	} else {
		picked_size = atoi(size_browser->text(fs));
		size_input->value(size_browser->text(fs));
	}

	const char *style = style_browser->text(style_browser->value());
	bool is_regular = (strcmp(style, "Regular") == 0);

	snprintf(face_buf, sizeof(face_buf), "%s %s %i",
			 font_browser->text(font_browser->value()),
			 is_regular ? "" : style,
			 picked_size);

	/*
	 * search it again, as 'find()' will register it as FLTK font
	 *
	 * TODO: this isn't too slow as database page is already loaded in memory, but as
	 * we already have current_font, maybe it would not be bad to add explicit font
	 * registration from FontInfo inside FontCache class.
	 */
	if(font_cache->find(face_buf, picked_font, picked_size)) {
		preview_input->textfont(picked_font);
		preview_input->textsize(picked_size);
		preview_input->redraw();
	} else
		E_WARNING(E_STRLOC ": failed to find '%s' font\n", face_buf);
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
	if(style_browser->value() < 1)
		style_browser->value(1);

	size_browser->clear();

	const char *style = style_browser->text(style_browser->value());
	bool is_regular = (strcmp(style, "Regular") == 0);

	snprintf(face_buf, sizeof(face_buf), "%s %s 12",
			 font_browser->text(font_browser->value()),
			 is_regular ? "" : style);

	int sz;
	FontInfo *fc = font_cache->find(face_buf, sz);

	/*
	 * safe to use it globally as it is not changed anywhere else; also
	 * allow it to be NULL so other callbacks knows this font does not exists
	 * in cache
	 */
	current_font = fc;

	if(!fc) return;

	/* dummy, use 'face_buf' to store number temporarily */
	for(int i = 0; i < fc->nsizes; i++) {
		snprintf(face_buf, sizeof(face_buf), "%i", fc->sizes[i]);
		size_browser->add(face_buf);
	}

	/* let size_cb find matching font size, select it and perform preview */
	size_cb(0, 0);
}

static void font_cb(Fl_Widget*, long) {
	/* Fl_Browser starts from 1 */
	if(font_browser->value() < 1)
		font_browser->value(1);

	style_browser->clear();

	const char *face = font_browser->text(font_browser->value());
	if(face) {
		int sz;
		style_browser->add("Regular");

		/*
		 * append italic/bold so and query FontCache for font availability
		 * also, append some size so FontCache doesn't complain
		 */
		snprintf(face_buf, sizeof(face_buf), "%s bold 12", face);
		if(font_cache->find(face_buf, sz) != NULL)
			style_browser->add("Bold");

		snprintf(face_buf, sizeof(face_buf), "%s italic 12", face);
		if(font_cache->find(face_buf, sz) != NULL)
			style_browser->add("Italic");

		snprintf(face_buf, sizeof(face_buf), "%s bold italic 12", face);
		if(font_cache->find(face_buf, sz) != NULL)
			style_browser->add("Bold Italic");
	}

	/* always select the first one */
	style_browser->value(1);

	/* let style_cb handle the rest */ 
	style_cb(0, 0);
}

static void load_font(const char *n, FontInfo *fi, void *data) {
	if(fi->type == 0)
		font_browser->add(n);
}

int font_chooser(const char* name, const char* family, int& retsize, const char* default_name, int default_size) {
	picked_size = DEFAULT_SIZE;
	picked_font = 0;
	picked_style = 0;
	current_font = NULL;

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

	/* load fonts from cache */
	font_cache = new FontCache();
	if(font_cache->load() && font_cache->count() > 0) {
		font_cache->for_each_font_sorted(load_font);

		/* sanity checks; should never happen */
		//E_ASSERT(font_family_size < nfonts && "font_family_size bigger than the sum of all fonts!!!");

		font_browser->value(1);
		size_browser->value(DEFAULT_SIZE);
		size_input->value(DEFAULT_SIZE_STR);
	
		/* now update the widgets */
		font_cb(0, 0);
	}

	win->show();
	while(win->shown())
		Fl::wait();

	if(retsize)
		retsize = picked_size;

	delete font_cache;
	return picked_font;
}

EDELIB_NS_END
