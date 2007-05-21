/*
 * $Id$
 *
 * ComboBox widget
 * Part of edelib.
 * Copyright (c) 2000-2007 EDE Authors.
 *
 * This program is licenced under terms of the
 * GNU General Public Licence version 2 or newer.
 * See COPYING for details.
 */

#include <edelib/ComboBox.h>

#include <fltk/damage.h>
#include <fltk/draw.h>
#include <fltk/events.h>
#include <fltk/run.h>
#include <fltk/Item.h>
#include <fltk/Box.h>
#include <fltk/Group.h>

#include <stdio.h>

EDELIB_NAMESPACE {

ComboBox::ComboBox(int x, int y, int w, int h, const char* l) : fltk::Menu(x,y,w,h,l), edt(false), sel(0), inpt(NULL) {
	clear_flag(fltk::ALIGN_MASK);
	set_flag(fltk::ALIGN_LEFT);
	box(fltk::DOWN_BOX);
	color(fltk::WHITE);

	// enable click_to_focus() below
	set_click_to_focus();

	set_editable();
}

ComboBox::~ComboBox() {
	if(inpt)
		delete inpt;
}

void ComboBox::set_editable(void) {
	if(!inpt) {
		int pad = w()-h()*4/5;

		// pretend as group so Input can be drawn correctly
		fltk::Group* g = current();
		current(0);

		inpt = new fltk::Input(x(), y(), pad, h());
		inpt->box(fltk::NO_BOX);
		inpt->parent(this);

		current(g);

		clear_click_to_focus();
	}
}

void ComboBox::draw_static(void) {
	if(damage() & fltk::DAMAGE_ALL)
		draw_frame();
	
	// position of the right box
	Rectangle br(w(),h());
	br.set_x(w()-h()*4/5);
	box()->inset(br);

	// draw box at the right
	if(damage() & (fltk::DAMAGE_ALL|fltk::DAMAGE_HIGHLIGHT)) {
		drawstyle(style(), (flags()|fltk::OUTPUT)&~fltk::FOCUSED);
		draw_glyph(fltk::ALIGN_BOTTOM|fltk::ALIGN_INSIDE, br);
	}

	if(damage() & (fltk::DAMAGE_ALL|fltk::DAMAGE_VALUE)) {
		// draw the rest
		Rectangle b(br.x(), h());
		box()->inset(b);

		fltk::setcolor(color());
		fltk::fillrect(b);

		if(flag(fltk::FOCUSED)) {
			fltk::setcolor(selection_color());
			b.inset(2); // compatibility (see below)
			fltk::fillrect(b);
		}

		// draw text
		if(sel) {
			fltk::Item::set_style(this,false);
			fltk::Flags saved = sel->flags();

			if(focused())
				sel->set_flag(fltk::SELECTED);
			else 
				sel->clear_flag(fltk::SELECTED);

			if (any_of(fltk::INACTIVE|fltk::INACTIVE_R)) 
				sel->set_flag(fltk::INACTIVE_R);

			// left for compatibility with fltk::ComboBox, fltk::Choice
			b.move_x(2);

			fltk::push_clip(b);
			fltk::push_matrix();
			if (!sel->h()) 
				sel->layout();

			// center item verticaly
			int h = sel->h();
			int n = h/(int)(sel->labelsize()+sel->leading());
			if (n > 1) 
				h -= int((n-1)*sel->labelsize()+(n-1.5)*sel->leading());

			fltk::translate(b.x(), b.y()+((b.h()-h)/2));

			// resize poped menu as size of label
			int save_w = sel->w(); 
			sel->w(b.w());
			sel->draw();

			fltk::Item::clear_style();
			sel->w(save_w);
			sel->flags(saved);
			fltk::pop_matrix();
			fltk::pop_clip();
		}
	}
}

void ComboBox::draw(void) {
	if(damage() & fltk::DAMAGE_ALL)
		draw_frame();

	// draw box at the right
	if(damage() & fltk::DAMAGE_ALL) {
		Rectangle br(w(),h());
		br.set_x(w()-h()*4/5);
		box()->inset(br);

		drawstyle(style(), (flags()|fltk::OUTPUT)&~fltk::FOCUSED);
		draw_glyph(fltk::ALIGN_BOTTOM|fltk::ALIGN_INSIDE, br);
	}

	inpt->set_damage(damage() | inpt->damage());
	if(inpt->damage()) {
		fltk::push_matrix();
		fltk::translate(inpt->x(), inpt->y());
		inpt->color(color());
		inpt->draw();
		fltk::pop_matrix();
		inpt->set_damage(0);
	}
}

void ComboBox::layout(void) {
	if(!inpt)
		return;

	int ww = h()*4/5;
	Rectangle rr(w() - ww, h());
	box()->inset(rr);
	inpt->resize(rr.x(), rr.y(), rr.w(), rr.h());
}

int ComboBox::handle_static(int event) {
	switch(event) {
		case fltk::FOCUS:
		case fltk::UNFOCUS:
			redraw(fltk::DAMAGE_VALUE);
			return 1;

		case fltk::ENTER:
		case fltk::LEAVE:
			redraw_highlight();
			return 1;

		case fltk::PUSH:
			if(click_to_focus()) {
				take_focus();
				fltk::flush();

				if(fltk::focus() != this)
					return 1;
			}

			if(!children(0,0))
				return 1;

			value(0);
			sel = try_popup(Rectangle(0, h()-2, w(), h()), 0);

			if(sel)
				redraw(fltk::DAMAGE_VALUE);
			return 1;
	}
	return fltk::Menu::handle(event);
}

int ComboBox::handle(int event) {
	inpt->when(when());
	int ret = 0;

	switch(event) {
		case fltk::PASTE:
		case fltk::TIMEOUT:
		case fltk::DND_ENTER:
		case fltk::DND_DRAG:
		case fltk::DND_LEAVE:
		case fltk::DND_RELEASE:
		case fltk::FOCUS_CHANGE:
			ret = inpt->handle(event);
			break;
		case fltk::FOCUS:
			ret = inpt->take_focus();
			break;

		case fltk::ACTIVATE:
		case fltk::ENTER:
		case fltk::SHOW:
			ret = handle_static(event);
			ret |= inpt->handle(event);
			break;

		case fltk::UNFOCUS:
		case fltk::DEACTIVATE:
		case fltk::HIDE:
		case fltk::LEAVE:
			ret = inpt->handle(event);
			ret |= handle_static(event);
			break;

		case fltk::PUSH:
			if(inpt && fltk::event_x() < w()-h()*4/5)
				ret = inpt->send(event);
			else {
				ret = handle_static(event);
				if(sel)
					inpt->value(sel->label());
			}
			break;
	}

	return ret;
}

}
