/*
 * $Id$
 *
 * Message dialog
 * Part of edelib.
 * Copyright (c) 2005-2007 EDE Authors.
 *
 * This program is licensed under terms of the 
 * GNU General Public License version 2 or newer.
 * See COPYING for details.
 */

#ifndef __MESSAGEBOX_H__
#define __MESSAGEBOX_H__

#include "econfig.h"

#include <FL/Fl_Window.h>
#include <FL/Fl_Button.h>
#include <FL/Fl_Box.h>
#include <FL/Fl_Input.h>
#include <FL/Fl_Pixmap.h>

#define MSGBOX_MAX_BUTTONS 4 // Max buttons in dialog

#define MSGBOX_ICON_ERROR    "dialog-error"
#define MSGBOX_ICON_INFO     "dialog-information"
#define MSGBOX_ICON_PASSWORD "dialog-password"
#define MSGBOX_ICON_WARNING  "dialog-warning"
#define MSGBOX_ICON_QUESTION "dialog-question"

EDELIB_NS_BEGIN

/**
 * \enum MessageBoxType
 * \brief Type of MessageBox dialog
 */
enum MessageBoxType {
	MSGBOX_PLAIN = 0,             ///< Plain dialog
	MSGBOX_INPUT,                 ///< Dialog with input field
	MSGBOX_INPUT_SECRET           ///< Dialog with secret input field
};

/**
 * \enum MessageBoxButtonType
 * \brief Type of added button to MessageBox class
 */
enum MessageBoxButtonType {
	MSGBOX_BUTTON_PLAIN = 0,      ///< Ordinary button (Fl_Button)
	MSGBOX_BUTTON_RETURN          ///< Button with 'enter' shortcut (Fl_Return_Button)
};

/**
 * \class MessageBox
 * \brief Standard dialog
 *
 * MessageBox is class for quickly building ordianary dialogs with
 * applied layout. This means following: it will resize to fit larger text input
 * or it will resize added buttons (to fit their labels), applying that
 * to whole window too.
 *
 * This class is not meant to be used to construct complicated dialogs nor
 * to construct dialogs with different elements or layout.
 *
 * So how to be used? MessageBox is by default MSGBOX_PLAIN, which means
 * that is ordianary dialog without any input. And here is the code:
 * \code
 *    MessageBox mb;
 *    // add some message
 *    mb.set_text("This is sample text");
 *    // add button
 *    mb.add_button("&Close");
 *    // show dialog
 *    mb.run_plain();
 * \endcode
 *
 * This sample will run dialog with provided text and one 'Close' button.
 * By default, this button will not have any callback attached to it, so
 * adding is done like:
 * \code
 *    ...
 *    mb.add_button("&Close", MSGBOX_BUTTON_PLAIN, some_callback_func);
 *    ...
 * \endcode
 * Now, clicking on 'Close' button, it will call <em>some_callback_func</em>.
 * Passing data to callback function is done via fltk style, like:
 * \code
 *    ...
 *    // send 'some_param' to callback
 *    mb.add_button("&Close", MSGBOX_BUTTON_PLAIN, some_callback_func, some_param);
 *    ...
 * \endcode
 *
 * add_button() can accept previously declared Fl_Button with already set
 * callback or properties so you can add it like:
 * \code
 *    ...
 *    Fl_Button* b = new Fl_Button(...);
 *    b->callback(...);
 *    mb.add_button(b);
 * \endcode
 *
 * If you want to add Fl_Return_Button (button with 'enter' as shortcut), using
 * <em>MSGBOX_BUTTON_RETURN</em> parameter will do the job, like:
 * \code
 *    ...
 *    mb.add_button("&Close", MSGBOX_BUTTON_RETURN, ...);
 *    // or already pre-allocated
 *    Fl_Return_Button* b = new Fl_Return_Button(...);
 *    mb.add_button(b, MSGBOX_BUTTON_RETURN);
 * \endcode
 *
 * \note If you added pre-allocated Fl_Button or Fl_Return_Button, <b>make sure</b>
 *       it is not deleted somewhere in the code since MessageBox will do that. What this
 *       means ? This means that added Fl_Button or Fl_Return_Button <b>must not</b> be
 *       inside begin()/end() functions, nor added to some group via add() or data will
 *       be deleted twice, crashing program probably.
 *
 * Adding more buttons is done via calling add_button() multiple times. You can add
 * max 4 buttons to dialog. This is dialog with 'Yes' and 'No' buttons:
 * \code
 *    MessageBox mb;
 *    mb.set_text("Would you like to quit");
 *    mb.add_button("&No", ...);
 *    mb.add_button("&Yes", ...);
 *    mb.run_plain();
 * \endcode
 * When multiple buttons are added, they should be added in reverse order, which means
 * that first added button will be at right edge of dialog and any further added will
 * be placed toward left edge.
 *
 * If you want dialog with input field (max. 1 input field is allowed), this is how:
 * \code
 *    MessageBox mb(MSGBOX_INPUT);
 *    mb.set_text("Please input something");
 *    mb.add_button("&Close me", ...);
 *    mb.run_plain();
 *
 *    // when dialog is closed, getting input is like
 *    printf("You entered %s\n", mb.get_input());
 * \endcode
 *
 * get_input() will return NULL if nothing was entered or if MSGBOX_PLAIN was set.
 *
 * Here is full sample of dialog requesting some password, where typed data is hidden
 * with asterisks:
 * \code
 *    void close_cb(Fl_Widget*, void* b) {
 *      MessageBox* mb = (MessageBox*)b;
 *      b->hide();
 *    }
 *
 *    // somewhere in the code
 *    MessageBox mb(MSGBOX_INPUT_SECRET);
 *    mb.set_text("Please enter password");
 *    mb.add_button("&Close", MSGBOX_BUTTON_PLAIN, close_cb, &mb);
 *    mb.run_plain();
 *
 *    const char* ret = mb.get_input();
 *    if(ret)
 *       printf("You entered %s\n", ret);
 *    else
 *       printf("Nothing was entered");
 * \endcode
 *
 * Setting callbacks each time just to get some status can be cumbersome, so there is
 * a run() function which is a shortcut for run_plain() with callbacks attached to each
 * button. This function will close dialog and return number of pressed button (starting 
 * from most right and 0); in case dialog was closed without pressing on any button
 * (like calling hide() or closing it via window manager) it will return -1.
 */
class MessageBox : public Fl_Window {
	private:
		Fl_Box* img;
		Fl_Box* txt;
		Fl_Input* inpt;
		Fl_Group* gr;
		Fl_Pixmap* pix;

		MessageBoxType mbt;

		int nbuttons;
		int b_start;
		Fl_Button* buttons[MSGBOX_MAX_BUTTONS];

		void init(void);
		void fix_sizes(void);

		void add(Fl_Widget*) { } // in case someone tries to be too smart

	public:
		/**
		 * Constructor which initialize internal data
		 * \param t is MessageBoxType type
		 */
		MessageBox(MessageBoxType t = MSGBOX_PLAIN);

		/**
		 * Clears internal data
		 */
		~MessageBox();

		/**
		 * Set message text
		 * \param txt is message text
		 */
		void set_text(const char* txt);

		/**
		 * Set icon giving absolute path
		 * \return true if icon was able to set
		 * \param path is full path to icon
		 */
		bool set_icon(const char* path);

		/**
		 * Set icon using loaded theme. Given icon name should not
		 * have an extension, nor should have path in it's name.
		 * \return true if icon was found
		 * \param name is icon name (without path and extension)
		 */
		bool set_theme_icon(const char* name);

		/**
		 * Set XPM icon. Parameter should be pointer to XPM array.
		 */
		void set_xpm_icon(const char* const* arr);

		/**
		 * Returns value from input field. Returned pointer points to internal storage
		 * and that storage is available during MessageBox instance life or untill clear()
		 * is called.
		 *
		 * Returned pointer value can be NULL when nothing was entered or MessageBox is type
		 * of MSGBOX_PLAIN.
		 */
		const char* get_input(void);

		/**
		 * Set default value to input field. Does nothing if dialog is MSGBOX_PLAIN type.
		 */
		void set_input(const char* txt);

		/**
		 * Focus a button. If number is greater than added buttons, it will to nothing.
		 */
		void focus_button(int b);

		/**
		 * Add button to dialog.
		 * \param b is already allocated button
		 * \param bt is button type
		 */
		void add_button(Fl_Button* b, MessageBoxButtonType bt = MSGBOX_BUTTON_PLAIN);

		/**
		 * Add button to dialog.
		 * \param l is button label
		 * \param bt is button type
		 * \param cb is callback for button
		 * \param param is data sent to callback
		 */
		void add_button(const char* l, MessageBoxButtonType bt = MSGBOX_BUTTON_PLAIN, Fl_Callback cb = 0, void* param = 0);

		/**
		 * Clears dialog and prepare internal data for next one.
		 * \param t is parameter telling how to prepare internal data for next dialog
		 */
		void clear(MessageBoxType t = MSGBOX_PLAIN);

		/**
		 * Runs dialog until called hide() or dialog was closed in normal way (clicking X in titlebar).
		 * \param center if set, dialog will be centered at the screen
		 */
		void run_plain(bool center = true);

		/**
		 * Runs dialog until pressed some of it's buttons or was called hide() on dialog. 
		 * \return -1 if nothing was pressed (but window was closed) or number of pressed button, starting
		 *    from 0. Also, buttons are counted from right (most right, if pressed will be 0, second will
		 *    be 1 and so).
		 * \param center if set, dialog will be centered at the screen
		 */
		int run(bool center = true);
};

/**
 * Set icon names used by IconTheme.
 * \related MessageBox
 * \param msg is icon for message()
 * \param alert is icon for alert()
 * \param ask is icon for ask()
 * \param input is icon for input()
 * \param password is icon for password()
 */
void themed_dialog_icons(const char* msg, const char* alert = 0, 
	const char* ask = 0, const char* input = 0, const char* password = 0);

/**
 * Clear icon names used by IconTheme.
 */
void clear_dialog_icons(void);

/**
 * Display message dialog in printf form.
 * \related MessageBox
 */
void message(const char* fmt, ...);

/**
 * Display dialog with alert message; in printf form.
 * \related MessageBox
 */
void alert(const char* fmt, ...);

/**
 * Display question dialog. Return 1 if user clicked 'Yes' or 0 if
 * user clicked 'No' or closed dialog.
 * \related MessageBox
 */
int ask(const char* fmt, ...);

/**
 * Display dialog with input field with 'OK' and 'Cancel' buttons.
 * If 'OK' is pressed (but input field is not empty), it will return
 * pointer to that data. Pointer is pointing to static memory and must
 * not be freed.
 *
 * If 'Cancel' was pressed (or dialog closed without pressing any buttons)
 * it will return NULL.
 * \related MessageBox
 */
const char* input(const char* fmt, const char* deflt = 0, ...);

/**
 * Same as input(), but typed characters are hidden with asterisks.
 * \related MessageBox
 */
const char* password(const char* fmt, const char* deflt = 0, ...);

#ifndef SKIP_DOCS
/*
 * FLTK compatibility
 */
extern void (*fl_message)(const char* fmt, ...);
extern void (*fl_alert)(const char* fmt, ...);
extern int  (*fl_ask)(const char* fmt, ...);
extern const char* (*fl_input)(const char* fmt, const char* deflt, ...);
extern const char* (*fl_password)(const char* fmt, const char* deflt, ...);
#endif

EDELIB_NS_END

#endif
