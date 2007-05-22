/*
 * $Id$
 *
 * Item widget
 * Part of edelib.
 * Copyright (c) 2000-2007 EDE Authors.
 *
 * This program is licenced under terms of the
 * GNU General Public Licence version 2 or newer.
 * See COPYING for details.
 */

// of course, code is mostly base from fltk version ((c) is already known for that case :))

#include <edelib/Item.h>

#include <fltk/draw.h>
#include <fltk/Box.h>

EDELIB_NAMESPACE {

Item::Item(const char* l) : fltk::Item(l), off_x_left(0), off_x_right(0) { }
Item::Item(const char* l, const fltk::Symbol* s) : fltk::Item(l, s), off_x_left(0), off_x_right(0)  { }
Item::Item(const char* l, int shortcut, fltk::Callback* callback, void* user_data, int flags) :
	fltk::Item(l, shortcut, callback, user_data, flags), off_x_left(0), off_x_right(0)  { }

void Item::draw(void) {
	drawstyle(style(), flags() & ~fltk::OUTPUT);

	if(flag(fltk::SELECTED)) {
		fltk::setbgcolor(selection_color());
		fltk::setcolor(fltk::contrast(selection_textcolor(), fltk::getbgcolor()));
	}

	fltk::Rectangle r(w(),h());
	fltk::Box* curr_box = box();
	curr_box->draw(r);
	curr_box->inset(r);

	if(type()) {
		int gw = (int)((textsize()) + 2);
		fltk::Rectangle lr(r);
		lr.move_x(gw + 3 + off_x_left);
		lr.w(lr.w() + off_x_right);
		draw_label(lr, flags());
		draw_glyph(0, fltk::Rectangle(r.x() + 3, r.y() + ((r.h() - gw) / 2), gw, gw));
	} else {
		fltk::Rectangle lr(r);
		lr.move_x(off_x_left);
		lr.w(lr.w() + off_x_right);
		draw_label(lr, flags());
	}

	//focusbox()->draw(r);
}

void Item::layout(void) {
	if (w() && h()) 
		return;

	fltk::setfont(textfont(), textsize());
	int lw = 250, lh = 250;
	fltk::measure(label(), lw, lh);

	if(lw)
		lw += 6 + (int)(textsize()) / 2;

	if (type()) 
		lw += 15;

	if (image()) {
		int iw, ih;
		image()->measure(iw, ih);
		if(ih > lh) 
			lh = ih;
		lw += iw;
  	}
	w(lw + off_x_right);
	h(lh + (int)leading());

	fltk::Widget::layout();
}

}
