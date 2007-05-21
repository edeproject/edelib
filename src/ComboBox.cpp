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

#include <stdio.h>

EDELIB_NAMESPACE {

//ComboBox::ComboBox(int x, int y, int w, int h, const char* l) : fltk::Choice(x,y,w,h,l)
ComboBox::ComboBox(int x, int y, int w, int h, const char* l) : fltk::Menu(x,y,w,h,l)
{
	clear_flag(fltk::ALIGN_MASK);
	set_flag(fltk::ALIGN_LEFT);
	value(0);
	box(fltk::DOWN_BOX);
}

ComboBox::~ComboBox()
{
}

void ComboBox::draw(void) 
{
	if(damage() & fltk::DAMAGE_ALL) {
		draw_frame();
		drawstyle(style(), (flags()|fltk::OUTPUT)&~fltk::FOCUSED);
  	}

	if(damage() & (fltk::DAMAGE_ALL|fltk::DAMAGE_VALUE)) {
		// draw box at the right
		Rectangle br(w(),h());
		br.set_x(w()-h()*4/5);
		box()->inset(br);
		draw_glyph(fltk::ALIGN_BOTTOM|fltk::ALIGN_INSIDE, br);

		// draw rest
		Rectangle b(br.x(), h());
		box()->inset(b);

		if(flag(fltk::FOCUSED)) {
			fltk::setcolor(selection_color());
			fltk::fillrect(b);
		} else {
			fltk::setcolor(fltk::RED);
			fltk::fillrect(b);
		}

		//puts("XXXXXXXXXXXXXXXXXXX");
	}
/*
	if(damage() & fltk::DAMAGE_ALL)
		draw_box();

	fltk::Rectangle r(w(),h()); 
	box()->inset(r);

	int w1 = r.h()*4/5;
	r.move_r(-w1);

	// little mark at the right
	if(damage() & (fltk::DAMAGE_ALL|fltk::DAMAGE_HIGHLIGHT)) 
	{
		drawstyle(style(), flags() & ~fltk::FOCUSED | fltk::OUTPUT);
		fltk::Rectangle gr(r.r(), r.y(), w1, r.h());
		draw_glyph(fltk::ALIGN_BOTTOM|fltk::ALIGN_INSIDE, gr);
	}

	if(damage() & (fltk::DAMAGE_ALL|fltk::DAMAGE_VALUE)) 
	{
		fltk::setcolor(color());
		fltk::fillrect(r);
		if(flag(fltk::FOCUSED)) 
		{
			fltk::setcolor(selection_color());
			Rectangle fr(r); 
			fr.inset(2); 
			fillrect(fr);
		}

		fltk::Widget* o = get_item();
		if(o) 
		{
			fltk::Item::set_style(this,false);
			fltk::Flags saved = o->flags();

			if (focused()) 
				o->set_flag(fltk::SELECTED);
			else 
				o->clear_flag(fltk::SELECTED);

			if (any_of(fltk::INACTIVE|fltk::INACTIVE_R)) 
				o->set_flag(fltk::INACTIVE_R);

			r.move_x(2);
			fltk::push_clip(r);
			fltk::push_matrix();
			if (!o->h()) 
				o->layout();

			// make it center on only the first line of multi-line item:
			int h = o->h();
			int n = h/int(o->labelsize()+o->leading());
			if (n > 1) h -= int((n-1)*o->labelsize()+(n-1.5)*o->leading());
			// center the item vertically:
			fltk::translate(r.x(), r.y()+((r.h()-h)>>1));
			int save_w = o->w(); o->w(r.w());
			o->draw();
			fltk::Item::clear_style();
			o->w(save_w);
			o->flags(saved);
			fltk::pop_matrix();
			fltk::pop_clip();
		}
	}
*/
}

int ComboBox::handle(int event)
{
	switch(event)
	{
		case fltk::FOCUS:
		case fltk::UNFOCUS:
			redraw(fltk::DAMAGE_VALUE);
			return 1;

		case fltk::ENTER:
		case fltk::LEAVE:
			redraw_highlight();
			return 1;

		case fltk::PUSH:
			if(click_to_focus())
			{
				take_focus();
				fltk::flush();
				if(fltk::focus() != this)
					return 1;
			}
			if(!children())
				return 1;
			value(0);
			if(popup(Rectangle(0, h()-2, w(), h()), 0))
				redraw(fltk::DAMAGE_VALUE);
			return 1;
	}

	//return fltk::Choice::handle(event);
	return fltk::Menu::handle(event);
}

}
