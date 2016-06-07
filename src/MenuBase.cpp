//
// "$Id$"
//
// Common menu code for the Fast Light Tool Kit (FLTK).
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

// This is a base class for all items that have a menu:
//	MenuBaseBar, MenuBaseButton, Fl_Choice
// This provides storage for a menu item, functions to add/modify/delete
// items, and a call for when the user picks a menu item.

// More code in Menu_add.cpp

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <FL/Fl.H>

#include <edelib/MenuBase.h>
#include <edelib/Missing.h>

EDELIB_NS_BEGIN

// Set 'pathname' of specified menuitem
//    If finditem==NULL, mvalue() is used (the most recently picked menuitem)
//    Returns:
//       0 : OK
//      -1 : item not found (name="")
//      -2 : 'name' not large enough (name="")
//
#define SAFE_STRCAT(s) \
    { len += strlen(s); if ( len >= namelen ) { *name='\0'; return(-2); } else strcat(name,(s)); }
int MenuBase::item_pathname(char *name, int namelen, const MenuItem *finditem) const {
    int len = 0;
    finditem = finditem ? finditem : mvalue();    
    name[0] = '\0';
    for ( int t=0; t<size(); t++ ) {
        const MenuItem *m = &(menu()[t]);
	if ( m->submenu() ) {				// submenu? descend
	    if (*name) SAFE_STRCAT("/");
	    if (m->label()) SAFE_STRCAT(m->label());
	} else {
	    if (m->label()) {				// menu item?
		if ( m == finditem ) {			// found? tack on itemname, done.
		    SAFE_STRCAT("/");
		    SAFE_STRCAT(m->label());
		    return(0);
		}
	    } else {					// end of submenu? pop
	        char *ss = strrchr(name, '/');
		if ( ss ) { *ss = 0; len = strlen(name); }	// "File/Edit" -> "File"
		else { name[0] = '\0'; len = 0; }		// "File" -> ""
		continue;
	    }
	}
    }
    *name = '\0';
    return(-1);						// item not found
}

// FIND MENU ITEM INDEX, GIVEN MENU PATHNAME
//     eg. "Edit/Copy"
//     Will also return submenus, eg. "Edit"
//     Returns NULL if not found.
//
const MenuItem *
MenuBase::find_item(const char *name)
{
  char menupath[1024] = "";	// File/Export

  for ( int t=0; t < size(); t++ ) {
    MenuItem *m = menu_ + t;

    if (m->flags&FL_SUBMENU) {
      // IT'S A SUBMENU
      // we do not support searches through FL_SUBMENU_POINTER links
      if (menupath[0]) edelib_strlcat(menupath, "/", sizeof(menupath));
      edelib_strlcat(menupath, m->label(), sizeof(menupath));
      if (!strcmp(menupath, name)) return m;
    } else {
      if (!m->label()) {
	// END OF SUBMENU? Pop back one level.
	char *ss = strrchr(menupath, '/');
	if ( ss ) *ss = 0;
	else menupath[0] = '\0';
	continue;
      }

      // IT'S A MENU ITEM
      char itempath[1024];	// eg. Edit/Copy
      strcpy(itempath, menupath);
      if (itempath[0]) edelib_strlcat(itempath, "/", sizeof(itempath));
      edelib_strlcat(itempath, m->label(), sizeof(itempath));
      if (!strcmp(itempath, name)) return m;
    }
  }

  return (const MenuItem *)0;
}

int MenuBase::value(const MenuItem* m) {
  clear_changed();
  if (value_ != m) {value_ = m; return 1;}
  return 0;
}

// When user picks a menu item, call this.  It will do the callback.
// Unfortunatly this also casts away const for the checkboxes, but this
// was necessary so non-checkbox menus can really be declared const...
const MenuItem* MenuBase::picked(const MenuItem* v) {
  if (v) {
    if (v->radio()) {
      if (!v->value()) { // they are turning on a radio item
	set_changed();
	((MenuItem*)v)->setonly();
      }
      redraw();
    } else if (v->flags & FL_MENU_TOGGLE) {
      set_changed();
      ((MenuItem*)v)->flags ^= FL_MENU_VALUE;
      redraw();
    } else if (v != value_) { // normal item
      set_changed();
    }
    value_ = v;
    if (when()&(FL_WHEN_CHANGED|FL_WHEN_RELEASE)) {
      if (changed() || when()&FL_WHEN_NOT_CHANGED) {
	if (value_ && value_->callback_) value_->do_callback((Fl_Widget*)this);
	else do_callback();
      }
    }
  }
  return v;
}

// turn on one of a set of radio buttons
void MenuItem::setonly() {
  flags |= FL_MENU_RADIO | FL_MENU_VALUE;
  MenuItem* j;
  for (j = this; ; ) {	// go down
    if (j->flags & FL_MENU_DIVIDER) break; // stop on divider lines
    j++;
    if (!j->text || !j->radio()) break; // stop after group
    j->clear();
  }
  for (j = this-1; ; j--) { // go up
    if (!j->text || (j->flags&FL_MENU_DIVIDER) || !j->radio()) break;
    j->clear();
  }
}

MenuBase::MenuBase(int X,int Y,int W,int H,const char* l)
: Fl_Widget(X,Y,W,H,l) {
  set_flag(SHORTCUT_LABEL);
  box(FL_UP_BOX);
  when(FL_WHEN_RELEASE_ALWAYS);
  value_ = menu_ = 0;
  alloc = 0;
  selection_color(FL_SELECTION_COLOR);
  textfont(FL_HELVETICA);
  textsize((uchar)FL_NORMAL_SIZE);
  textcolor(FL_FOREGROUND_COLOR);
  down_box(FL_NO_BOX);
}

int MenuBase::size() const {
  if (!menu_) return 0;
  return menu_->size();
}

void MenuBase::menu(const MenuItem* m) {
  clear();
  value_ = menu_ = (MenuItem*)m;
}

// this version is ok with new MenuBaseadd code with fl_menu_array_owner:

void MenuBase::copy(const MenuItem* m, void* ud) {
  int n = m->size();
  MenuItem* newMenu = new MenuItem[n];
  memcpy(newMenu, m, n*sizeof(MenuItem));
  menu(newMenu);
  alloc = 1; // make destructor free array, but not strings
  // for convienence, provide way to change all the user data pointers:
  if (ud) for (; n--;) {
    if (newMenu->callback_) newMenu->user_data_ = ud;
    newMenu++;
  }
}

MenuBase::~MenuBase() {
  clear();
}

// Fl_Menu::add() uses this to indicate the owner of the dynamically-
// expanding array.  We must not free this array:
MenuBase* fl_menu_array_owner = 0;

void MenuBase::clear() {
  if (alloc) {
    if (alloc>1) for (int i = size(); i--;)
      if (menu_[i].text) free((void*)menu_[i].text);
    if (this == fl_menu_array_owner)
      fl_menu_array_owner = 0;
    else
      delete[] menu_;
    menu_ = 0;
    value_ = 0;
    alloc = 0;
  }
}

EDELIB_NS_END
