/*
 * $Id$
 *
 * Group with applied layout on childs
 * Copyright (c) 2005-2013 edelib authors
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

#include <FL/fl_draw.H>
#include <FL/Fl.H>

#include <edelib/ExpandableGroup.h>
#include <edelib/Debug.h>

#define CHILD_OFFSET  10
#define BORDER_OFFSET 10

EDELIB_NS_BEGIN

static void vscroll_cb(Fl_Widget*, void* w) {
	ExpandableGroup* eg = (ExpandableGroup*)w;
	eg->scrolly(eg->get_scroll()->value());
}

ExpandableGroup::ExpandableGroup(int X, int Y, int W, int H, const char* l) :
	Fl_Group(X, Y, W, H, l), scroll_w(16), px(0), py(0), sval(0), sval_curr(0), sval_old(0)
{
#if (FL_MAJOR_VERSION >= 1) && ((FL_MINOR_VERSION >= 3) || ((FL_MINOR_VERSION >= 1) && (FL_PATCH_VERSION >= 10)))
	scroll_w = Fl::scrollbar_size();
#endif

	area_x = X + BORDER_OFFSET;
	area_y = Y + BORDER_OFFSET;
	area_w = W;
	area_h = H;

	begin();
	vscrollbar = new Fl_Scrollbar(X + W - scroll_w, Y, scroll_w, H);
	vscrollbar->hide();
	vscrollbar->callback(vscroll_cb, this);
	end();
}

void ExpandableGroup::draw_clip(void* d, int X, int Y, int W, int H) {
	ExpandableGroup *eg = (ExpandableGroup*)d;

	fl_clip(X, Y, W, H);
	fl_color(eg->color());
	fl_rectf(X, Y, W, H);

	Fl_Widget *o;
	for(int i = 0; i < eg->children(); i++) {
		o = eg->child(i);
		eg->draw_child(*o);
		eg->draw_outside_label(*o);
	}

	fl_pop_clip();
}

void ExpandableGroup::draw(void) {
	int X,Y,W,H;

	X = x() + Fl::box_dx(box());
	Y = y() + Fl::box_dy(box());
	W = w() - Fl::box_dw(box());
	H = h() - Fl::box_dh(box());

	unsigned char d = damage();
	if(d & FL_DAMAGE_ALL) {
		draw_box(box() ? box() : FL_DOWN_BOX , x(), y(), w(), h(), color());
		draw_clip(this, X, Y, W, H);
	}

	if(d & FL_DAMAGE_SCROLL) {
		/* no filickering */
		fl_scroll(X, Y, W - vscrollbar->w(), H, 0, sval_old - sval_curr, draw_clip, this);
	}

	/* must update childs too */
	if(d & FL_DAMAGE_CHILD) {
		fl_clip(X, Y, W, H);

		Fl_Widget *o;
		for(int i = 0; i < children(); i++) {
			o = child(i);
			update_child(*o);
		}

		fl_pop_clip();
	}

	(d & FL_DAMAGE_ALL) ? draw_child(*vscrollbar) : update_child(*vscrollbar);
}

int ExpandableGroup::handle(int event) {
	switch(event) {
		case FL_SHOW:
			reposition_childs();
			return 1;
		case FL_KEYBOARD:
			if(Fl::event_key() == FL_Enter || Fl::event_key() == FL_KP_Enter) {
				do_callback();
				return 1;
			}
	}

	return Fl_Group::handle(event);
}

void ExpandableGroup::reposition_childs(void) {
	if(children() < 1) return;

	Fl_Widget *o = child(0);
	if(o == vscrollbar)
		o = child(1);

	/* update area */
	area_x = x() + BORDER_OFFSET;
	area_y = y() + BORDER_OFFSET;
	area_w = w() - BORDER_OFFSET;
	area_h = BORDER_OFFSET + o->h();

	/* init sizes */
	px = area_x;
	py = area_y;
	py -= sval;

	int ch_size = children();
	for(int i = 0; i < ch_size; i++) {
		if(!child(i)->visible())
			continue;
		if(child(i) == vscrollbar)
			continue;
		if((px + child(i)->w()) <= area_x + area_w) {
			/* ok, widget is not greater than total width */
			child(i)->position(px, py);
		} else {
			/* widget goes out of bounds, set it to new row setting x to begin and increasing y */
			px = area_x;
			py += child(i)->h() + CHILD_OFFSET;
			child(i)->position(px, py);

			/* increase widget_area height since is used for scrolling */
			area_h += child(i)->h() + CHILD_OFFSET;
		}

		/* we always increase this since widgets flow left to right */
		px += child(i)->w() + CHILD_OFFSET;
	}

	/* now add down border */
	area_h += BORDER_OFFSET;

	/*
	 * count how many childs are fully visible not overlapping BORDER_OFFSET
	 * BORDER_OFFSET is already calculated in widget_area
	 */ 
	int vis = 0;
	for(int i = 0; i < children(); i++) {
		if(((child(i)->y() + child(i)->h()) <= (h() - BORDER_OFFSET)) && (child(i)->y() >= BORDER_OFFSET))
			vis++;
	}

	if(vis != children()) {
		/* update scrollbar */
		vscrollbar->resize(x() + w() - scroll_w, y(), scroll_w, h());
		vscrollbar->value(sval, h(), 0, area_h);
		/* now show it */
		vscrollbar->set_visible();
	} else {
		vscrollbar->hide();
	}

	sval = 0;
}

void ExpandableGroup::resize(int X, int Y, int W, int H) {
	Fl_Widget::resize(X,Y,W,H);
	reposition_childs();
	Fl_Group::init_sizes();
}

void ExpandableGroup::scrolly(int ypos) {
	sval = ypos;
	sval_old = sval_curr;
	sval_curr = ypos;
	reposition_childs();
	damage(FL_DAMAGE_SCROLL);
}

/* overriden so we don't remove scrollbar */
void ExpandableGroup::clear(void) {
	Fl_Widget *sc;
	for(int i = children(); i--;) {
		sc = child(i);
		remove(sc);
		delete sc;
	}

	area_w = w() - BORDER_OFFSET;
	area_h = h() - BORDER_OFFSET;

	sval = sval_curr = sval_old = 0;
}

/* set scrollbar last so iterations can skip it */
void ExpandableGroup::fix_scrollbar_order(void) {
	int all = Fl_Group::children();

	if(child(all-1) == vscrollbar)
		return;

	Fl_Widget **arr = (Fl_Widget**)array();
	int i = 0, j = 0;
	for(; j < all; j++) {
		if(arr[j] != vscrollbar)
			arr[i++] = arr[j];
	}

	arr[i++] = vscrollbar;
}

/*
 * This will return actuall size - 1, which is scrollbar by default. In all internal code is used so
 * we don't check explicitly against scrollbar pointer. Also external usage will skip it.
 */
int ExpandableGroup::children(void) {
	int ch = Fl_Group::children();
	if(child(ch - 1) != vscrollbar)
		fix_scrollbar_order();

	return Fl_Group::children() - 1;
}

void ExpandableGroup::add(Fl_Widget* o) {
	Fl_Group::add(o);
	if(visible())
		reposition_childs();
}

EDELIB_NS_END
