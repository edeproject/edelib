//
// "$Id$"
//
// Menu bar header file for the Fast Light Tool Kit (FLTK).
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

#ifndef __MENUBAR_H__
#define __MENUBAR_H__

#include "MenuBase.h"

EDELIB_NS_BEGIN

/**
 * \class MenuBar
 * \brief Menu bar
 *
 * MenuBar is forked Fl_Menu_Bar, so all FLTK documentation for Fl_Menu_Bar applies here too.
 */
class EDELIB_API MenuBar : public MenuBase {
protected:
#ifndef SKIP_DOCS
	void draw();
#endif
public:
	/**
	 * Constructor
	 */
	MenuBar(int X, int Y, int W, int H,const char *l=0) : MenuBase(X,Y,W,H,l) {}

	/**
	 * Handle events
	 */
	int handle(int);
};

EDELIB_NS_END
#endif
