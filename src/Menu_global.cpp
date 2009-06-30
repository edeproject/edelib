//
// "$Id$"
//
// Global menu shortcut code for the Fast Light Tool Kit (FLTK).
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

// Make all the shortcuts in this menu global.
// Currently only one menu at a time and you cannot destruct the menu,
// is this sufficient?

#include <FL/Fl.H>
#include <edelib/MenuBase.h>

EDELIB_NS_BEGIN

static MenuBase* the_widget;

static int handler(int e) {
  if (e != FL_SHORTCUT || Fl::modal()) return 0;
  Fl::first_window(the_widget->window());
  return the_widget->handle(e);
}

void MenuBase::global() {
  if (!the_widget) Fl::add_handler(handler);
  the_widget = this;
}

EDELIB_NS_END
