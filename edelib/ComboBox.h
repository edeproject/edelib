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

#ifndef __COMBOBOX_H__
#define __COMBOBOX_H__

#include "econfig.h"

#include <fltk/Choice.h>
#include <fltk/Menu.h>

EDELIB_NAMESPACE {

//class ComboBox : public fltk::Choice
class ComboBox : public fltk::Menu
{
	public:
		/**
		 * Constructs an empty group.
		 */
		ComboBox(int x, int y, int w, int h, const char* l=0);

		/**
		 * Clear data and call destructor on widgets.
		 */
		~ComboBox();

#ifndef SKIP_DOCS
		virtual void draw(void);
		virtual int handle(int event);
#endif
};

}
#endif
