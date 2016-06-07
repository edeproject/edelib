/*
 * $Id$
 *
 * Group with applied layout on childs
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

#ifndef __EDELIB_EXPANDABLEGROUP_H__
#define __EDELIB_EXPANDABLEGROUP_H__

#include "edelib-global.h"

#include <FL/Fl_Group.H>
#include <FL/Fl_Scrollbar.H>

EDELIB_NS_BEGIN

/**
 * \defgroup widgets FLTK widgets and functions
 */

/**
 * \ingroup widgets
 * \class ExpandableGroup
 * \brief A group with applied layout on childs.
 *
 * ExpandableGroup is group where all childs are placed next to each other. Resizing this group will dynamically
 * reorder childs position so they fit into group height. Because of this, childs x() and y() values are ignored.
 *
 * Starting position for the first child is x = 10, y = 10, and spacing between each added child is 10 px.
 *
 * Childs before be put in, <b>must</b> be allocated with <em>new</em>; destructor on them will be called automatically.
 *
 * \note All childs have to have the same height, so reordering can be correctly computed
 */

class EDELIB_API ExpandableGroup : public Fl_Group {
private:
	int scroll_w;
	int px, py;
	int sval, sval_curr, sval_old;
	int area_x, area_y, area_w, area_h;

	Fl_Scrollbar *vscrollbar;

	void reposition_childs(void);
	void fix_scrollbar_order(void);
	static void draw_clip(void *d, int X, int Y, int W, int H);

public:
	/**
	 * Constructs an empty group.
	 */
	ExpandableGroup(int x, int y, int w, int h, const char *l = 0);

#ifndef SKIP_DOCS
	virtual void draw(void);
	virtual void resize(int x, int y, int w, int h);
	virtual int handle(int event);
	void clear(void);
	int children(void);
	void scrolly(int yp);
	void add(Fl_Widget* o);
	Fl_Scrollbar* get_scroll(void) { return vscrollbar; }
#endif
};

EDELIB_NS_END
#endif
