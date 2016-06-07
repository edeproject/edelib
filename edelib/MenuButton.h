//
// "$Id$"
//
// Menu button header file for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2005 by Bill Spitzak and others.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.

#ifndef __EDELIB_MENUBUTTON_H__
#define __EDELIB_MENUBUTTON_H__

#include "MenuBase.h"

EDELIB_NS_BEGIN

/**
 * \class MenuButton
 * \brief Button with attached menu
 *
 * MenuButton is forked Fl_Menu_Button, so all FLTK documentation for Fl_Menu_Button applies here too.
 */
class EDELIB_API MenuButton : public MenuBase {
protected:
#ifndef SKIP_DOCS
	void draw();
#endif
public:
	/**
	 * Constructor
	 */
	MenuButton(int,int,int,int,const char * =0);

	/**
	 * Values for type()
	 */
	enum {POPUP1 = 1, POPUP2, POPUP12, POPUP3, POPUP13, POPUP23, POPUP123};

	/**
	 * Handle events
	 */
	int handle(int);

	/**
	 * Popup menu and return the menu item or NULL if the user dismisses the menu
	 */
	const MenuItem* popup();
};

EDELIB_NS_END
#endif
