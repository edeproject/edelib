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

#ifndef __EXPANDABLEGROUP_H__
#define __EXPANDABLEGROUP_H__

#include "econfig.h"

#include <fltk/Group.h>
#include <fltk/Scrollbar.h>
#include <fltk/Rectangle.h>

EDELIB_NAMESPACE {

/**
 * \class ExpandableGroup
 * A group with applied layout on childs.
 *
 * ExpandableGroup is group where all childs are placed
 * next to each other. Resizing this group will dynamically
 * reorder childs position so they fit into group height.
 * Because of this, childs x() and y() values are ignored.
 *
 * Starting position for the first child is x = 10, y = 10, and spacing
 * between each added child is 10 px.
 *
 * \note All childs have to have the same height, so reordering can be correctly computed
 */

class ExpandableGroup : public fltk::Group
{
	private:
		int px, py;
		int sval;
		fltk::Scrollbar* vscrollbar;
		fltk::Rectangle widget_area;

	public:
		ExpandableGroup(int x, int y, int w, int h, const char* l=0);
		~ExpandableGroup();
		void draw(void);
		void layout(void);
		void scrolly(int yp);
		fltk::Scrollbar& get_scroll(void) { return *vscrollbar; }
};
}
#endif
