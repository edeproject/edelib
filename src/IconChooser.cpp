/*
 * $Id$
 *
 * Icon choose dialog
 * Part of edelib.
 * Copyright (c) 2000-2007 EDE Authors.
 *
 * This program is licenced under terms of the
 * GNU General Public Licence version 2 or newer.
 * See COPYING for details.
 */

#include <edelib/IconChooser.h>
#include <edelib/Directory.h>
#include <edelib/Vector.h>
#include <edelib/ExpandableGroup.h>
#include <edelib/Nls.h>
#include <edelib/Debug.h>
#include <edelib/StrUtil.h>

#include <fltk/Window.h>
#include <fltk/run.h>
#include <fltk/Button.h>
#include <fltk/Input.h>
#include <fltk/InvisibleBox.h>
#include <fltk/SharedImage.h>
#include <fltk/Group.h>
#include <fltk/file_chooser.h>
#include <fltk/ProgressBar.h>
#include <fltk/events.h>

// maximums for icon sizes
#define MAX_ICON_W  128
#define MAX_ICON_H  128

EDELIB_NAMESPACE {

inline int MAX(int a, int b) { return a > b ? a : b; }

class IconBox : public fltk::Button
{
	private: 
		fltk::Color corig;
		String iconpth;
	public:
		IconBox(int x, int y, int w, int h, const char* l=0);
		~IconBox();
		int handle(int event);
		void set_icon_path(const String& s) { iconpth.assign(s); }
		const String& icon_path(void) const { return iconpth; }
};

IconBox::IconBox(int x, int y, int w, int h, const char* l) : fltk::Button(x, y, w, h, l)
{
	box(fltk::FLAT_BOX);
	corig = color();
	iconpth = "";
}

IconBox::~IconBox()
{
}

int IconBox::handle(int event)
{
	switch(event)
	{
		case fltk::FOCUS:
			color(selection_color());
			redraw();
			return 1;
		case fltk::UNFOCUS:
			color(corig);
			redraw();
			return 1;
		case fltk::PUSH:
			take_focus();
			// double-click
			if(fltk::event_clicks())
				do_callback();
			return 1;
		case fltk::RELEASE:
			return 1;
		default:
			return fltk::Button::handle(event);
	}
	return 1;
}

class IconChooser : public fltk::Window
{
	private:
		String ret;
		String start;

		fltk::Input* path;
		fltk::Button* bbrowse;
		fltk::Button* bok;
		fltk::Button* bcancel;
		fltk::ProgressBar* progress;
		ExpandableGroup* icongrp;

	public:
		IconChooser();
		~IconChooser();
		void load(const char* dir);
		bool find_focused(void);

		String get_ret(void)    { return ret; }
		String& get_start(void) { return start; }
};

void cancel_cb(fltk::Widget*, void* w)
{
	IconChooser* ic = (IconChooser*)w;
	ic->hide();
}

void ok_cb(fltk::Widget*, void* w)
{
	IconChooser* ic = (IconChooser*)w;
	if(ic->find_focused())
		ic->hide();
}

void browse_cb(fltk::Widget*, void* w)
{
	IconChooser* ic = (IconChooser*)w;
	const char* dd = fltk::dir_chooser(_("Choose icon directory..."), ic->get_start().c_str(), false);
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
void iconbox_cb(fltk::Widget*, void* w)
{
	ok_cb(NULL, w);
}

IconChooser::IconChooser() : fltk::Window(355, 305, _("Choose icon...")), ret("")
{
	begin();
	path = new fltk::Input(10, 15, 240, 25);
	bbrowse = new fltk::Button(255, 15, 90, 25, _("&Browse..."));
	bbrowse->callback(browse_cb, this);

	icongrp = new ExpandableGroup(10, 45, 335, 220);
	icongrp->box(fltk::DOWN_BOX);
	icongrp->color(fltk::WHITE);

	progress = new fltk::ProgressBar(10, 270, 110, 25);
	progress->minimum(0);
	progress->hide();

	bok = new fltk::Button(160, 270, 90, 25, _("&OK"));
	bok->callback(ok_cb, this);
	bcancel = new fltk::Button(255, 270, 90, 25, _("&Cancel"));
	bcancel->callback(cancel_cb, this);

	// invisible resizable box
	fltk::InvisibleBox* ibox = new fltk::InvisibleBox(10, 240, 110, 25);
	resizable(ibox);
	end();
}

IconChooser::~IconChooser()
{
}

void IconChooser::load(const char* dir)
{
	EASSERT(dir != NULL);
	/*
	 * copy directory name to input box and internal String 
	 * so it can be reused later
	 */
	path->value(dir);
	start.assign(dir);

	if(!dir_exists(dir))
		return;

	vector<String> lst;
	if(!dir_list(dir, lst, true))
		return;

	/*
	 * TODO: replace this stupid FLTK hack
	 * with add_handler()
	 */
	fltk::register_images();

	/*
	 * SharedImage does not calculate dimensions
	 * at first load, until data was cached (read as: >= second load). Since
	 * we have to have dimensions now (so can calculate box size)
	 * Image::measure() will do the job. Yes, it's not too fast, but 
	 * how to extract header data without direct mess with dependant libraries ?
	 *
	 * This have to be done with two pass; in first we find maximal dimensions
	 * and those are used as final for preview boxes; second will draw actual image.
	 */
	fltk::SharedImage* img = NULL;
	int imax_w = 0;
	int imax_h = 0;
	int iw, ih;
	bool show_progress = false;

	/*
	 * lst_info contains coresponding indexes with vector<String> so we can deduce what
	 * files to skip (not readable image or dimensions greater than allowed); skippable
	 * are marked as 0
	 */
	int* lst_info = new int[lst.size()];

	for(unsigned int i = 0; i < lst.size(); i++)
	{
		img = fltk::SharedImage::get(lst[i].c_str());
		if(!img)
		{
			lst_info[i] = 0;
			continue;
		}

		img->measure(iw, ih);

		if(iw > MAX_ICON_W || ih > MAX_ICON_H)
		{
			lst_info[i] = 0;
			continue;
		}

		imax_w = MAX(imax_w, iw);
		imax_h = MAX(imax_h, ih);
		lst_info[i] = 1;
	}

	// clear potential content of ExpandableGroup
	if(icongrp->children())
	{
		icongrp->clear();
		icongrp->redraw();
		icongrp->relayout();
	}

	if(lst.size() > 10)
	{
		show_progress = true;
		progress->range(0, lst.size(), 1);
		progress->show();
	}

	imax_w += 5;
	imax_h += 5;

	/*
	 * focus_index() is only valid on childs before we show them 
	 * and that is what we need so other childs don't mess it when
	 * they are added
	 */
	icongrp->focus_index(0);

	for(unsigned int i = 0; i < lst.size(); i++)
	{
		img = fltk::SharedImage::get(lst[i].c_str());
		if(img && lst_info[i] == 1)
		{
			IconBox* preview = new IconBox(0, 0, imax_w, imax_h);
			preview->set_icon_path(lst[i]);
			// use background from ExpandableGroup
			preview->color(icongrp->color());

			if(show_progress)
				progress->position(int((i*100)/int(progress->maximum())));

			preview->image(img);
			preview->callback(iconbox_cb, this);
			icongrp->add(preview);

			fltk::check();
		}
	}

	progress->hide();

	delete [] lst_info;
}

bool IconChooser::find_focused(void)
{
	for(int i = 0; i < icongrp->children(); i++)
	{
		if(icongrp->child(i)->focused())
		{
			IconBox* ib = (IconBox*)icongrp->child(i);
			ret.assign(ib->icon_path());
			return true;
		}
	}
	return false;
}

String icon_chooser(const char* dir)
{
	IconChooser ic;
	ic.load(dir);

	ic.show();
	while(ic.visible())
		fltk::wait();

	return ic.get_ret();
}

}
