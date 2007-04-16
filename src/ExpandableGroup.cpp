/*
 * $Id$
 *
 * Group with applied layout on childs
 * Part of edelib.
 * Copyright (c) 2000-2007 EDE Authors.
 *
 * This program is licenced under terms of the
 * GNU General Public Licence version 2 or newer.
 * See COPYING for details.
 */

#include <edelib/econfig.h>
#include <edelib/ExpandableGroup.h>
#include <fltk/layout.h>
#include <fltk/draw.h>
#include <fltk/damage.h>

#define SLIDER_WIDTH  16

#define CHILD_OFFSET  10
#define BORDER_OFFSET 10

EDELIB_NAMESPACE {

void vscroll_cb(fltk::Widget*, void* w) 
{
	ExpandableGroup* eg = (ExpandableGroup*)w;
	eg->scrolly(eg->get_scroll().value());
}

ExpandableGroup::ExpandableGroup(int x, int y, int w, int h, const char* l) : 
	fltk::Group(x, y, w, h, l), px(0), py(0), sval(0)//, vscrollbar(x + w - SLIDER_WIDTH, y, SLIDER_WIDTH, h)
{
	end();
	//vscrollbar(x + w - SLIDER_WIDTH, y, SLIDER_WIDTH, h)
	vscrollbar = new fltk::Scrollbar(x + w - SLIDER_WIDTH, y, SLIDER_WIDTH, h);
	vscrollbar->set_vertical();
	vscrollbar->hide();
	vscrollbar->callback(vscroll_cb, this);

	widget_area.x(BORDER_OFFSET);
	widget_area.y(BORDER_OFFSET);
}

ExpandableGroup::~ExpandableGroup()
{
}

void ExpandableGroup::draw() 
{
	clear_flag(fltk::HIGHLIGHT);
	/*
	 * this will remove DAMAGE checks so
	 * we can have correct clipping;
	 * this will slow drawing things, but we
	 * will not get overlaped childs over group bounds;
 	 *
	 * NOTE: we clip it againt group bounds, not widget_area
	 * since it is helper for childs placement
	 */

	if(damage() & fltk::DAMAGE_ALL) 
	{
		draw_box();
		draw_label();
	}

	fltk::push_clip(0, 2, w()-3, h()-4);

	if(damage() & (fltk::DAMAGE_ALL | fltk::DAMAGE_CHILD)) 
	{
		for(int n = children(); n--;) 
		{
			Widget& w = *child(n);
			draw_child(w);
			if(w.damage() & fltk::DAMAGE_CHILD_LABEL)
			{
				draw_outside_label(w);
				w.set_damage(w.damage() & ~fltk::DAMAGE_CHILD_LABEL);
			}
		}
	}

	fltk::pop_clip();

	/*
	 * draw scrollbar and prevent childs overwrite it
	 * TODO: this can be fixed increasing right bound
	 * when scroll is visible
	 */
	if(damage() & (fltk::DAMAGE_ALL | fltk::DAMAGE_CHILD))
		vscrollbar->set_damage(fltk::DAMAGE_ALL);
}

void ExpandableGroup::layout() 
{
	if(!layout_damage()) 
		return;

	if (!(layout_damage()&(fltk::LAYOUT_WH|fltk::LAYOUT_DAMAGE))) 
	{
		Widget::layout();
		return;
	}

	if(children() < 2)
	{
		// for cases when we call clear() on this group
		vscrollbar->hide();
		return;
	}

	// update area width to group width
	widget_area.w(w()-BORDER_OFFSET);

	/*
	 * Assume first child have correct height
	 * and expand area with it's size.
	 *
	 * widget_area have role to emulate group
	 * full height, just as all childs are visible; this
	 * is needed so scrollbar can be used.
	 */
	widget_area.h(BORDER_OFFSET + child(0)->h());

	// init sizes
	px = widget_area.x();
	py = widget_area.y();
	py = py - sval;

	for(int i = 0; i < children(); i++) 
	{
		if(!child(i)->visible())
			continue;

		if((px + child(i)->w()) <= widget_area.w()) 
		{
			// ok, widget is not greater than total width
			child(i)->x(px);
			child(i)->y(py);
		}
		else 
		{
			/*
			 * widget goes out of bounds, set it to new row
			 * setting x to begin and increasing y
			 */
			px = widget_area.x();
			py += child(i)->h() + CHILD_OFFSET;
			child(i)->x(px);
			child(i)->y(py);

			// increase widget_area height since is used for scrolling
			widget_area.h( widget_area.h() + child(i)->h() + CHILD_OFFSET );
		}
		// we always increase this since widgets flow left to right
		px += child(i)->w() + CHILD_OFFSET;
	}

	// now add down border
	widget_area.h( widget_area.h() + BORDER_OFFSET );

	/*
	 * count how many childs are fully visible not overlapping BORDER_OFFSET
	 * so we can show scrollbar if needed
	 */
	int vis = 0;
	for(int i = 0; i < children(); i++) {
		if(((child(i)->y() + child(i)->h()) <= (h() - BORDER_OFFSET)) && (child(i)->y() >= BORDER_OFFSET))
			vis++;
	}

	if(vis != children()) 
	{
		// update scrollbar
		vscrollbar->position(x() + w() - SLIDER_WIDTH, y());
		vscrollbar->w(SLIDER_WIDTH);
		vscrollbar->h(h());
		vscrollbar->value(sval, h(), 0, widget_area.h());
		// now show it
		vscrollbar->set_visible();
	} 
	else
		vscrollbar->hide();

	sval = 0;
	set_damage(fltk::DAMAGE_CHILD);
	Widget::layout();
}

void ExpandableGroup::scrolly(int yp) 
{
	sval = yp;
	relayout();
}

}
