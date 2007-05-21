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
#include <fltk/Input.h>

EDELIB_NAMESPACE {

class ComboBox : public fltk::Menu
{
	private:
		bool edt;

		fltk::Widget* sel;
		fltk::Input*  inpt;

		void draw_static(void);
		int handle_static(int event);
	public:
		/**
		 * Constructs an empty group.
		 */
		ComboBox(int x, int y, int w, int h, const char* l=0);

		/**
		 * Clear data and call destructor on widgets.
		 */
		~ComboBox();

		void set_editable(void);
		bool editable(void)     { return edt; }

#ifndef SKIP_DOCS
		virtual void draw(void);
		virtual void layout(void);
		virtual int handle(int event);
#endif
};

}
#endif
