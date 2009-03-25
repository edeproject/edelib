/*
 * $Id$
 *
 * Icon chooser
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

#include <FL/Fl_Window.H>
#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Shared_Image.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Progress.H>
#include <FL/fl_draw.H>

#include <edelib/IconChooser.h>
#include <edelib/IconLoader.h>
#include <edelib/Directory.h>
#include <edelib/ExpandableGroup.h>
#include <edelib/Nls.h>
#include <edelib/Debug.h>
#include <edelib/StrUtil.h>

/* max icon sizes */
#define MAX_ICON_W  128
#define MAX_ICON_H  128

#undef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))

EDELIB_NS_BEGIN

class IconBox : public Fl_Button {
private: 
	bool sel;
	Fl_Color corig;
	String iconpth;
	char* iconname;
public:
	IconBox(int x, int y, int w, int h, const char* l=0);
	~IconBox();

	void set_icon_path(const String& s);
	const String& icon_path(void) const { return iconpth; }
	bool selected(void) { return sel; }

	int handle(int event);
};

IconBox::IconBox(int x, int y, int w, int h, const char* l) : Fl_Button(x, y, w, h, l) {
	box(FL_FLAT_BOX);
	corig = color();
	iconpth = "";
	sel = false;
	iconname = NULL;
}

IconBox::~IconBox() { 
	free(iconname);
}

void IconBox::set_icon_path(const String& s) {
	char* ptr;
	int W = 0, H = 0, len;

	iconpth.assign(s);

	// get basename without extension
	ptr = strrchr(s.c_str(), E_DIR_SEPARATOR);
	if(ptr) {
		ptr += 1;
		char* ptr2 = strrchr(ptr, '.');
		if(ptr2)
			*ptr2 = '\0';
	} else {
		ptr = _("(unknown)");
	}

	iconname = strdup(ptr);
	len = strlen(iconname);

	fl_measure(iconname, W, H);

	if(W > w() && len > 10) {
		// copy as label so we can modify it
		copy_label(iconname);
		ptr = (char*)(label() + 10);

		// end label string with '...'
		*ptr = '\0';
		*(ptr - 1) = '.';
		*(ptr - 2) = '.';
		*(ptr - 3) = '.';
	} else {
		label(iconname);
	}

	align(FL_ALIGN_INSIDE);
	tooltip(iconname);
}

/*
 * FIXME: icon will loose focus if is selected
 * then press OK, which will take focus from it.
 * In this case, nothing will be returned; double-click
 * works as expected.
 */
int IconBox::handle(int event) {
	switch(event) {
		case FL_FOCUS:
			corig = color();
			color(selection_color());
			redraw();
			sel = true;
			return 1;
		case FL_UNFOCUS:
			color(corig);
			redraw();
			sel = false;
			return 1;
		case FL_PUSH:
			take_focus();
			// double-click
			if(Fl::event_clicks())
				do_callback();
			return 1;
		case FL_RELEASE:
			return 1;
		default:
			return Fl_Button::handle(event);
	}
	return 1;
}

class IconChooser : public Fl_Window {
private:
	String ret;
	String start;

	Fl_Input* path;
	Fl_Button* bbrowse;
	Fl_Button* bok;
	Fl_Button* bcancel;
	Fl_Progress* progress;
	ExpandableGroup* icongrp;

public:
	IconChooser();
	~IconChooser();
	void load(const char* dir);
	void load_from_list(list<String>& lst);
	bool find_focused(void);

	String get_ret(void)    { return ret; }
	String& get_start(void) { return start; }
};

static void cancel_cb(Fl_Widget*, void* w) {
	IconChooser* ic = (IconChooser*)w;
	ic->hide();
}

static void ok_cb(Fl_Widget*, void* w) {
	IconChooser* ic = (IconChooser*)w;
	if(ic->find_focused())
		ic->hide();
}

static void browse_cb(Fl_Widget*, void* w) {
	IconChooser* ic = (IconChooser*)w;
	const char* dd = fl_dir_chooser(_("Choose icon directory..."), ic->get_start().c_str(), false);
	if(!dd)
		return;
	ic->load(dd);
}

/*
 * This callback is called when is double-clicked on
 * icon box inside icon list; since all childs when
 * are double-clicked get focus automatically, this
 * forwarding to ok_cb(), who checks what child is
 * focused, is valid
 */
void iconbox_cb(Fl_Widget*, void* w) {
	ok_cb(NULL, w);
}

IconChooser::IconChooser() : Fl_Window(355, 305, _("Choose icon...")), ret("") {
	begin();
	path = new Fl_Input(10, 10, 240, 25);
	bbrowse = new Fl_Button(255, 10, 90, 25, _("&Browse..."));
	bbrowse->callback(browse_cb, this);

	// invisible resizable box
	Fl_Box* ibox = new Fl_Box(15, 160, 115, 95);
	resizable(ibox);

	icongrp = new ExpandableGroup(10, 40, 335, 220);
	icongrp->box(FL_DOWN_BOX);
	icongrp->color(FL_WHITE);
	icongrp->end();

	progress = new Fl_Progress(10, 270, 125, 25);
	progress->minimum(0);
	progress->hide();

	bok = new Fl_Button(160, 270, 90, 25, _("&OK"));
	bok->callback(ok_cb, this);
	bcancel = new Fl_Button(255, 270, 90, 25, _("&Cancel"));
	bcancel->callback(cancel_cb, this);

	end();
}

IconChooser::~IconChooser() { }

void IconChooser::load(const char* dir) {
	E_ASSERT(dir != NULL);
	/*
	 * copy directory name to input box and internal String 
	 * so it can be reused later
	 */
	path->value(dir);
	start.assign(dir);

	if(!dir_exists(dir))
		return;

	list<String> lst;
	if(!dir_list(dir, lst, true))
		return;

	load_from_list(lst);
}

void IconChooser::load_from_list(list<String>& lst) {
	if(lst.empty())
		return;

	/*
	 * TODO: replace this this with
	 * something better
	 */
	fl_register_images();

	Fl_Shared_Image* img = NULL;
	int imax_w = 0;
	int imax_h = 0;
	int iw, ih;
	bool show_progress = false;

	list<String>::iterator it = lst.begin(), it_end = lst.end();

	/*
	 * lst_info contains coresponding indexes with list<String> so we can deduce what
	 * files to skip (not readable image or dimensions greater than allowed); skippable
	 * are marked as 0
	 */
	int* lst_info = new int[lst.size()];
	for(int i = 0; it != it_end; ++it, i++){
		img = Fl_Shared_Image::get((*it).c_str());

		if(!img) {
			lst_info[i] = 0;
			continue;
		}

		iw = img->w();
		ih = img->h();

		if(iw > MAX_ICON_W || ih > MAX_ICON_H) {
			lst_info[i] = 0;
			continue;
		}

		imax_w = MAX(imax_w, iw);
		imax_h = MAX(imax_h, ih);
		lst_info[i] = 1; 
	}

	// clear potential content of ExpandableGroup
	if(icongrp->children())
		icongrp->clear();

	if(lst.size() > 10) {
		show_progress = true;
		progress->minimum(0);
		progress->maximum(lst.size());
		progress->show();
	}

	if(imax_w < 64) 
		imax_w = 64;
	else
		imax_w += 10;

	if(imax_h < 64) 
		imax_h = 64;
	else
		imax_h += 10;

	imax_w += 5;
	imax_h += 5;

	/*
	 * focus_index() is only valid on childs before we show them 
	 * and that is what we need so other childs don't mess it when
	 * they are added
	 */
	//icongrp->focus(child(0));
	icongrp->set_visible_focus();

	IconBox* preview;
	it = lst.begin();
	for(int i = 0; it != it_end; ++it, i++) {
		img = Fl_Shared_Image::get((*it).c_str());

		if(img && lst_info[i] == 1) {
			preview = new IconBox(0, 0, imax_w, imax_h);
			preview->set_icon_path((*it));
			// use background/selection from ExpandableGroup
			preview->color(icongrp->color());
			preview->selection_color(icongrp->color());

			if(show_progress)
				progress->value(int((i*100)/int(progress->maximum())));

			preview->image(img);
			preview->callback(iconbox_cb, this);
			icongrp->add(preview);

			Fl::check();
		}
	}

	progress->hide();
	delete [] lst_info;
}

bool IconChooser::find_focused(void) {
	IconBox* ib;
	for(int i = 0; i < icongrp->children(); i++) {
		ib = (IconBox*)icongrp->child(i);

		if(ib->selected()) {
			ret.assign(ib->icon_path());
			return true;
		}
	}
	return false;
}

String icon_chooser(const char* dir) {
	IconChooser ic;
	ic.load(dir);

	ic.show();
	while(ic.visible())
		Fl::wait();

	return ic.get_ret();
}

String icon_chooser(IconSizes sz, IconContext ctx) {
	IconChooser ic;

	if(IconLoader::inited()) {
		list<String> all;

		const IconTheme* t = IconLoader::theme();
		E_ASSERT(t != NULL && "IconLoader loaded, but IconTheme object NULL");

		t->query_icons(all, sz, ctx);
		ic.load_from_list(all);
	}

	ic.show();
	while(ic.visible())
		Fl::wait();

	return ic.get_ret();
}

EDELIB_NS_END
