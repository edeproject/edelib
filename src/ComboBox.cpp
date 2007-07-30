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

#include <string.h> // strlen, strcmp, strncmp

#define MIN_CHARS 6  // min. chars to start autocomplete

EDELIB_NS_BEGIN

void input_cb(fltk::Widget*, void* w) {
	ComboBox* cbox = (ComboBox*)w;
	cbox->try_autocomplete();
}

ComboBox::ComboBox(int x, int y, int w, int h, const char* l) : 
	fltk::Menu(x,y,w,h,l), ctype(COMBOBOX_STATIC), min_match(MIN_CHARS), sel(NULL), curr(NULL), inpt(NULL) {

	clear_flag(fltk::ALIGN_MASK);
	set_flag(fltk::ALIGN_LEFT);
	box(fltk::DOWN_BOX);
	color(fltk::WHITE);

	// enable click_to_focus() below
	set_click_to_focus();

	//when(fltk::WHEN_CHANGED);
}

ComboBox::~ComboBox() {
	if(inpt)
		delete inpt;
}

void ComboBox::type(int t) {
	if(ctype == t)
		return;

	ctype = t;
	if(ctype & COMBOBOX_STATIC) {
		if(inpt)
			delete inpt;
		set_click_to_focus();
		return;
	} else {
		if(!inpt) {
			int pad = w()-h()*4/5;

			// pretend as group so Input can be drawn correctly
			fltk::Group* g = current();
			current(0);

			inpt = new fltk::Input(x(), y(), pad, h());
			inpt->box(fltk::NO_BOX);
			inpt->parent(this);

			if(ctype & COMBOBOX_AUTOCOMPLETE) {
				inpt->callback(input_cb, this);
				inpt->when(fltk::WHEN_CHANGED);
			}

			current(g);
			clear_click_to_focus();
		}
	}
}

void ComboBox::draw(void) {
	if(!inpt)
		draw_static();
	else
		draw_editable();
}

int ComboBox::handle(int event) {
	if(!inpt)
		return handle_static(event);
	else
		return handle_editable(event);
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

void ComboBox::draw_editable(void) {
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
	// inset() leave some space from right so w()+2 is enough to hide them
	rr.w(rr.w()+2);
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

		case fltk::SHOW:
			/*
			 * childs are build in runtime so this can't be checked
			 * in constructor or similar
			 */
			if(children()) {
				// for cases when value() is given before widget is shown
				if(value() < 0)
					value(0);

				sel = child(0);
				redraw(fltk::DAMAGE_VALUE);
			}
			return 1;

		case fltk::PUSH:
			if(click_to_focus()) {
				take_focus();
				fltk::flush();

				if(fltk::focus() != this)
					return 1;
			}

	EXECUTE:
			if(!children(0,0))
				return 1;

			value(0);
			/*
			 * Make a copy in case menu is popped and nothing is selected
			 * but window is moved
			 */
			curr = try_popup(Rectangle(0, h()-2, w(), h()), 0);
			if(curr) {
				sel = curr;
				redraw(fltk::DAMAGE_VALUE);
				// run callback
				if(sel->takesevents())
					execute(sel);
			}
			return 1;

		case fltk::SHORTCUT:
			if(test_shortcut())
				goto EXECUTE;
			if(handle_shortcut()) {
				redraw(fltk::DAMAGE_VALUE);
				return 1;
			}
			return 0;
	}
	return fltk::Menu::handle(event);
}

int ComboBox::handle_editable(int event) {
	/*
	 * enabling this will enable ComboBox::when() (in constructor uncomment too)
	 * inpt->when(when());
	 */
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
				// pick up value and fill input box
				if(sel) {
					inpt->text(sel->label());
					inpt->take_focus();
				}
			}
			break;
	}

	return ret;
}

// valid only if set_editable() is used
void ComboBox::try_autocomplete(void) {
	if(!inpt)
		return;

	if(!(ctype & COMBOBOX_AUTOCOMPLETE))
		return;

	const char* str = inpt->text();
	int sz = strlen(str);
	if(sz < min_match)
		return;

	fltk::Widget* found = 0;

	// let's rock
	for(int i = 0; i < children(); i++) {
		if(strncmp(child(i)->label(), str, sz) == 0) {
			found = child(i);
			break;
		}
	}

	if(found)
		inpt->text(found->label());
}

void ComboBox::choice(unsigned int it) {
	bool ret = value(it);
	if(ret) {
		fltk::Widget* ww = get_item();

		if(inpt)
			inpt->text(ww->label());
		else
			sel = ww;
	}
}

int ComboBox::choice(void) {
	fltk::Widget* ww = get_item();
	if(!ww || !ww->label())
		return -1;
	if(!inpt)
		return fltk::Menu::value();
	if(strcmp(ww->label(), inpt->text()) == 0)
		return fltk::Menu::value();
	else {
		// last resort, scan all childs
		for(int i = 0; i < children(); i++) {
			if(child(i)->label() && (strcmp(child(i)->label(), inpt->text()) == 0))
				return i;
		}
	}

	return -1;
}

const char* ComboBox::text(void) { 
	if(inpt)
		return inpt->text();
	else if(sel)
		return sel->label();
	return 0;
}

EDELIB_NS_END
