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

#include <FL/Fl_Group.h>
#include <FL/Fl_Scrollbar.h>

EDELIB_NS_BEGIN

/**
 * \class ExpandableGroup
 * \brief A group with applied layout on childs.
 *
 * ExpandableGroup is group where all childs are placed
 * next to each other. Resizing this group will dynamically
 * reorder childs position so they fit into group height.
 * Because of this, childs x() and y() values are ignored.
 *
 * Starting position for the first child is x = 10, y = 10, and spacing
 * between each added child is 10 px.
 *
 * Childs before be put in, <b>must</b> be allocated with <em>new</em>;
 * destructor on them will be called automatically.
 *
 * \note All childs have to have the same height, so reordering can be correctly computed
 */

class EDELIB_API ExpandableGroup : public Fl_Group {
	private:
		int px, py;
		int sval, sval_curr, sval_old;
		int area_x, area_y, area_w, area_h;

		Fl_Scrollbar*  vscrollbar;

		void reposition_childs(void);
		void fix_scrollbar_order(void);
		static void draw_clip(void* d, int X, int Y, int W, int H);

	public:
		/**
		 * Constructs an empty group.
		 */
		ExpandableGroup(int x, int y, int w, int h, const char* l=0);

		/**
		 * Clear data and call destructor on widgets.
		 */
		~ExpandableGroup();

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
#endif // __EXPANDABLEGROUP_H__
