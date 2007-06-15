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

#ifndef __ITEM_H__
#define __ITEM_H__

#include "econfig.h"

#include <fltk/Item.h>

EDELIB_NAMESPACE {

/**
 * \class Item
 * \brief Menu or Browser entity
 *
 * Item is single entity of fltk::Menu or fltk::Browser. This class behaves
 * the same as fltk::Item with added few methods that affect label drawing
 */

class EDELIB_API Item : public fltk::Item
{
	private:
		int off_x_left;
		int off_x_right;

	public:
		/**
		 * Constructs an item with given label.
		 */
		Item(const char* l = 0);

		/**
		 * Constructs an item with given label and symbol.
		 */
		Item(const char* l, const fltk::Symbol* s);

		/**
		 * Constructs an item with given label, shortcut and callback.
		 */
		Item(const char* l, int shortcut, fltk::Callback* callback = 0, void* user_data = 0, int flags = 0);

		/**
		 * Set label offsets in widget. Parameter <em>right</em> can be omitted.
		 */
		void offset_x(int left, int right = 0) { off_x_left = left; off_x_right = right; }

		/**
		 * Return left offset.
		 */
		int offset_x_left(void) { return off_x_left; }

		/**
		 * Return right offset.
		 */
		int offset_x_right(void) { return off_x_right; }

#ifndef SKIP_DOCS
		virtual void draw(void);
		virtual void layout(void);
#endif
};

}
#endif
