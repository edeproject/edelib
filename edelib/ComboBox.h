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

enum {
	COMBOBOX_STATIC        = 1,        ///< Non-editable ComboBox (like fltk::Choice)
	COMBOBOX_EDITABLE      = 2,        ///< Editable ComboBox (like fltk::ComboBox)
	COMBOBOX_AUTOCOMPLETE  = 4         ///< Editable ComboBox with auto-completition
};

/**
 * \class ComboBox
 * \brief Combo box widget
 *
 * The Combo box widget allows to select one of options from list or to specify
 * a new one (but list will be unchanged).
 *
 * \image html combobox.jpg
 *
 * This implementation is mix between fltk::Choice and fltk::ComboBox, allowing in
 * one widget non-editable options and editable options with auto-completition when
 * editable is chosen.
 *
 * Filling ComboBox with items is the same as filling fltk's versions, like:
 * \code
 *   ComboBox* cb = new ComboBox(...);
 *   cb->type(COMBOBOX_STATIC);
 *   cb->begin();
 *     new fltk::Item(...);
 *     new fltk::Item(...);
 *     new fltk::Item(...);
 *   cb->end();
 * \endcode
 *
 * fltk::Item() can either be simple (label only) or with callback. For the details
 * about this, fltk documentation should be consulted. 
 * When callback is present, it will be executed when specified item is selected.
 *
 * On other hand, when ComboBox is in editable state, some functions (like choice())
 * can be functional only on known items; manualy editing option's label or providing
 * a new content, will result choice() returning -1 (which can be an indicator that something
 * newish is added). Sample:
 * \code
 *   cb->type(COMBOBOX_EDITABLE);
 *   cb->begin();
 *     new fltk::Item("Foo");
 *     new fltk::Item("Baz");
 *   cb->end();
 * \endcode
 * If user edit "Foo", choice() will return -1. Content can be inspected with text().
 *
 * For auto-complete version, default minimal size for match is 6 characters, but this
 * can be changed with match_size(). For auto-complete version the same applies as for editable
 * version, since is simple subset of it.
 */

class ComboBox : public fltk::Menu
{
	private:
		int  ctype;
		int  min_match;

		fltk::Widget* sel;
		fltk::Widget* curr;
		fltk::Input*  inpt;

		void draw_static(void);
		void draw_editable(void);
		int  handle_static(int event);
		int  handle_editable(int event);

	public:
		/**
		 * Constructs an empty widget.
		 */
		ComboBox(int x, int y, int w, int h, const char* l=0);

		/**
		 * Clear data and call destructor on widgets.
		 */
		~ComboBox();

		/**
		 * Set type of ComboBox (COMBOBOX_STATIC, COMBOBOX_EDITABLE or COMBOBOX_AUTOCOMPLETE)
		 */
		void type(int t);

		/**
		 * Return current ComboBox type.
		 */
		int type(void)          { return ctype; }

		/**
		 * Return true if ComboBox is editable.
		 */
		bool editable(void)     { return !(ctype & COMBOBOX_STATIC); }

		/**
		 * Set minimal number of characters to match for COMBOBOX_AUTOCOMPLETE. Default
		 * is 6 characters if not specified.
		 */
		void match_size(int s)  { min_match = s; }

		/**
		 * Get current minimal number of characters to match for COMBOBOX_AUTOCOMPLETE.
		 */
		int match_size(void)  { return min_match; }

		/**
		 * Select from list of items and show it in box. If number is greater than number
		 * of items, first will be used by default. Items are starting from 0.
		 */
		void choice(unsigned int it);

		/**
		 * Return currently selected item. If is not known (editable versions), it will return -1.
		 */
		int choice(void);

		/**
		 * Return label of selected item or character content if item is not known. Content is
		 * pointing to the internal buffer and should not be modified.
		 *
		 * If is unable to return content or found item have NULL label, it will return NULL.
		 */
		const char* text(void);

#ifndef SKIP_DOCS
		virtual void draw(void);
		virtual void layout(void);
		virtual int handle(int event);
		void try_autocomplete(void);
#endif
};

}
#endif
