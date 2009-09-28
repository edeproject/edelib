//
// "$Id$"
//
// Menu item header file for the Fast Light Tool Kit (FLTK).
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

#ifndef __EDELIB_MENUITEM_H__
#define __EDELIB_MENUITEM_H__

#include "edelib-global.h"

#include <FL/Fl_Widget.H>
#include <FL/Fl_Image.H>
#include <FL/Fl_Menu_Item.H>

#if defined(__APPLE__) && defined(check)
# undef check
#endif

EDELIB_NS_BEGIN

class MenuBase;

/**
 * \class MenuItem
 * \brief The item in menu list
 *
 * MenuItem is forked Fl_Menu_Item, so all FLTK documentation for Fl_Menu_Item applies here too.
 */
struct EDELIB_API MenuItem {
#ifndef SKIP_DOCS
	const char *text;	// label()
	int shortcut_;
	Fl_Callback *callback_;
	void *user_data_;
	int flags;
	uchar labeltype_;
	uchar labelfont_;
	uchar labelsize_;
	unsigned labelcolor_;

	// image used menu entries; ignored if entry is title
	// additional field can be used in struct generated from FLUID
	Fl_Image *image_;

	// advance N items, skipping submenus:
	const MenuItem *next(int=1) const;
	MenuItem *next(int i=1) { return (MenuItem*)(((const MenuItem*)this)->next(i)); }
	const MenuItem *first() const { return next(0); }
	MenuItem *first() { return next(0); }

	// methods on menu items:
	const char* label() const {return text;}
	void label(const char* a) {text=a;}
	void label(Fl_Labeltype a,const char* b) {labeltype_ = a; text = b;}
	Fl_Labeltype labeltype() const {return (Fl_Labeltype)labeltype_;}
	void labeltype(Fl_Labeltype a) {labeltype_ = a;}
	Fl_Color labelcolor() const {return (Fl_Color)labelcolor_;}
	void labelcolor(unsigned a) {labelcolor_ = a;}
	Fl_Font labelfont() const {return (Fl_Font)labelfont_;}
	void labelfont(uchar a) {labelfont_ = a;}
	uchar labelsize() const {return labelsize_;}
	void labelsize(uchar a) {labelsize_ = a;}
	Fl_Callback_p callback() const {return callback_;}
	void callback(Fl_Callback* c, void* p) {callback_=c; user_data_=p;}
	void callback(Fl_Callback* c) {callback_=c;}
	void callback(Fl_Callback0*c) {callback_=(Fl_Callback*)c;}
	void callback(Fl_Callback1*c, long p=0) {callback_=(Fl_Callback*)c; user_data_=(void*)p;}
	void* user_data() const {return user_data_;}
	void user_data(void* v) {user_data_ = v;}
	long argument() const {return (long)user_data_;}
	void argument(long v) {user_data_ = (void*)v;}
	int shortcut() const {return shortcut_;}
	void shortcut(int s) {shortcut_ = s;}
	int submenu() const {return flags&(FL_SUBMENU|FL_SUBMENU_POINTER);}
	int checkbox() const {return flags&FL_MENU_TOGGLE;}
	int radio() const {return flags&FL_MENU_RADIO;}
	int value() const {return flags&FL_MENU_VALUE;}
	void set() {flags |= FL_MENU_VALUE;}
	void clear() {flags &= ~FL_MENU_VALUE;}
	void setonly();
	int visible() const {return !(flags&FL_MENU_INVISIBLE);}
	void show() {flags &= ~FL_MENU_INVISIBLE;}
	void hide() {flags |= FL_MENU_INVISIBLE;}
	int active() const {return !(flags&FL_MENU_INACTIVE);}
	void activate() {flags &= ~FL_MENU_INACTIVE;}
	void deactivate() {flags |= FL_MENU_INACTIVE;}
	int activevisible() const {return !(flags&0x11);}

	// different from original Fl_Menu_Item
	void image(Fl_Image* a) { image_ = a; }
	void image(Fl_Image& a) { image_ = &a; }
	Fl_Image* image() const { return image_; }

	// used by menubar:
	int measure(int* h, const MenuBase*) const;
	void draw(int x, int y, int w, int h, const MenuBase*, int t=0, int label_gap=0) const;

	// popup menus without using an MenuBase widget:
	const MenuItem* popup(
	  int X, int Y,
	  const char *title = 0,
	  const MenuItem* picked=0,
	  const MenuBase* = 0) const;

	const MenuItem* pulldown(
	  int X, int Y, int W, int H,
	  const MenuItem* picked = 0,
	  const MenuBase* = 0,
	  const MenuItem* title = 0,
	  int menubar=0) const;

	const MenuItem* test_shortcut() const;
	const MenuItem* find_shortcut(int *ip=0) const;

	void do_callback(Fl_Widget* o) const {callback_(o, user_data_);}
	void do_callback(Fl_Widget* o,void* arg) const {callback_(o, arg);}
	void do_callback(Fl_Widget* o,long arg) const {callback_(o, (void*)arg);}

	// back-compatability, do not use:
	int checked() const {return flags&FL_MENU_VALUE;}
	void check() {flags |= FL_MENU_VALUE;}
	void uncheck() {flags &= ~FL_MENU_VALUE;}
	int add(const char*, int shortcut, Fl_Callback*, void* =0, int = 0);
	int add(const char*a, const char* b, Fl_Callback* c, void* d = 0, int e = 0) { return add(a,fl_old_shortcut(b),c,d,e); }
	int size() const ;
#endif
};

EDELIB_NS_END
#endif
