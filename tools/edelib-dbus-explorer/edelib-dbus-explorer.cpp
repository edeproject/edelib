/*
 * $Id: String.h 2839 2009-09-28 11:36:20Z karijes $
 *
 * Copyright (c) 2005-2012 edelib authors
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

#include <FL/Fl_Tile.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl.H>

#include <edelib/Debug.h>
#include <edelib/Nls.h>
#include <edelib/MenuItem.h>
#include <edelib/MenuBar.h>
#include <edelib/Ede.h>
#include <edelib/MessageBox.h>
#include <edelib/Window.h>

#include <edelib/EdbusConnection.h>
#include <edelib/EdbusMessage.h>
#include <edelib/EdbusList.h>

#include "ScriptEditor.h"
#include "ObjectTree.h"
#include "icons.h"

EDELIB_NS_USING_AS(Window, AppWindow)
EDELIB_NS_USING(MenuItem)
EDELIB_NS_USING(MenuBar)
EDELIB_NS_USING(ask)
EDELIB_NS_USING(alert)
EDELIB_NS_USING(message)
EDELIB_NS_USING(EdbusConnection)
EDELIB_NS_USING(EdbusMessage)
EDELIB_NS_USING(EdbusList)
EDELIB_NS_USING_LIST(2, (EDBUS_SESSION, EDBUS_SYSTEM))

#define PROGRAM_LABEL   "edelib DBus Explorer"
#define PROGRAM_VERSION "0.1"

/* on which bus we are connected */
#define CONNECTED_BUS_NONE    0
#define CONNECTED_BUS_SESSION 1
#define CONNECTED_BUS_SYSTEM  2

#define LOAD_SAVE_PATTERN "*.{ss,scm}"
	
static void quit_cb(Fl_Widget*, void*);
static void session_bus_cb(Fl_Widget*, void*);
static void system_bus_cb(Fl_Widget*, void*);
static void disconnect_cb(Fl_Widget*, void*);
static void about_cb(Fl_Widget*, void*);
static void save_cb(Fl_Widget*, void*);
static void save_as_cb(Fl_Widget*, void*);
static void load_cb(Fl_Widget*, void*);
static void editor_undo_cb(Fl_Widget*, void*);
static void editor_copy_cb(Fl_Widget*, void*);
static void editor_paste_cb(Fl_Widget*, void*);
static void editor_select_all_cb(Fl_Widget*, void*);
static void evaluate_cb(Fl_Widget*, void*);
static void evaluate_and_print_cb(Fl_Widget*, void*);
static void macro_expand_cb(Fl_Widget*, void*);

static MenuItem menu_[] = {
	{_("&File"), 0,  0, 0, 64, FL_NORMAL_LABEL, 0},
	{_("Connect to"), 0,  0, 0, 64, FL_NORMAL_LABEL, 0},
	{_("Session Bus"), 0,  session_bus_cb, 0, 0, FL_NORMAL_LABEL, 0},
	{_("System Bus"), 0,  system_bus_cb, 0, 0, FL_NORMAL_LABEL, 0},
	{0,0,0,0,0,0,0,0,0},
	{_("Disconnect"), 0,  disconnect_cb, 0, 128, FL_NORMAL_LABEL, 0},
	{_("Load..."), FL_COMMAND + 'o', load_cb, 0, 0, FL_NORMAL_LABEL, 0},
	{_("Save..."), FL_COMMAND + 's', save_cb, 0, 0, FL_NORMAL_LABEL, 0},
	{_("Save As..."), 0, save_as_cb, 0, 128, FL_NORMAL_LABEL, 0},
	{_("&Quit"), 0x40071, quit_cb, 0, 0, FL_NORMAL_LABEL, 0},
	{0,0,0,0,0,0,0,0,0},
	{_("&Edit"), 0, 0, 0, 64, FL_NORMAL_LABEL, 0},
	{_("&Undo"), FL_COMMAND + 'u', editor_undo_cb , 0, 128, FL_NORMAL_LABEL, 0},
	{_("&Copy"), FL_COMMAND + 'c', editor_copy_cb , 0, 0, FL_NORMAL_LABEL, 0},
	{_("&Paste"), FL_COMMAND + 'p', editor_paste_cb, 0, 128, FL_NORMAL_LABEL, 0},
	{_("&Select All"), FL_COMMAND + 'a', editor_select_all_cb, 0, 0, FL_NORMAL_LABEL, 0},
	{0,0,0,0,0,0,0,0,0},
	{_("&Run"), 0,  0, 0, 64, FL_NORMAL_LABEL, 0},
	{_("&Evaluate"), FL_SHIFT + FL_Enter,  evaluate_cb, 0, 0, FL_NORMAL_LABEL, 0},
	{_("E&valuate And Print"), FL_ALT + FL_Enter,  evaluate_and_print_cb, 0, 128, FL_NORMAL_LABEL, 0},
	{_("&Macro expand"), 0, macro_expand_cb, 0, 0, FL_NORMAL_LABEL, 0},
	{0,0,0,0,0,0,0,0,0},
	{_("&Help"), 0,  0, 0, 64, FL_NORMAL_LABEL, 0},
	{_("&About"), 0,  about_cb, 0, 0, FL_NORMAL_LABEL, 0},
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0}
};

static Fl_Output *status_bar;
static AppWindow *win;
static Fl_Hold_Browser *service_browser;
static ObjectTree *method_browser;
static ScriptEditor *script_editor;
static int connection_bus_type;
static EdbusConnection *bus_connection;
static const char *saved_content;

static void quit_cb(Fl_Widget*, void*) {
	if(ask(_("You are going to quit application. Please select 'Yes' to continue or 'No' to cancel this dialog."))) 
		win->hide();
}

static void list_bus_names(EdbusConnection *c, Fl_Hold_Browser *browser, bool show_all = false) {
	browser->clear();

	EdbusMessage m, reply;
	m.create_method_call("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus", "ListNames");

	if(!c->send_with_reply_and_block(m, 1000, reply)) {
		alert(_("Unable to get bus name list"));
		return;
	}

	/* we should get array of strings */
	EdbusMessage::const_iterator el = reply.begin();
	if(!el->is_array()) {
		alert(_("Got junk as reply from bus list. Please reconnect and try again"));
		return;
	}

	EdbusList array = el->to_array();
	EdbusList::const_iterator it = array.begin(), ite = array.end();
	for(; it != ite; ++it) {
		if(!it->is_string()) {
			E_WARNING(E_STRLOC ": Expected string as array element, but received some junk. Skipping...\n");
			continue;
		}

		if(!show_all && it->to_string()[0] == ':') continue;
		browser->add(it->to_string());

#if (FL_MAJOR_VERSION >= 1) && (FL_MINOR_VERSION >= 3)
		browser->icon(browser->size(), &image_service);
	}

	browser->sort();
#else
	}
#endif
}

static void session_bus_cb(Fl_Widget*, void*) {
	if(!bus_connection) bus_connection = new EdbusConnection();
	bus_connection->disconnect();

	if(!bus_connection->connect(EDBUS_SESSION)) {
		alert(_("Unable to connect to session bus. Is DBus daemon running?"));
		return;
	}

	status_bar->value(_("Connected to session bus"));
	connection_bus_type = CONNECTED_BUS_SESSION;

	list_bus_names(bus_connection, service_browser);
}

static void system_bus_cb(Fl_Widget*, void*) {
	if(!bus_connection) bus_connection = new EdbusConnection();
	bus_connection->disconnect();

	if(!bus_connection->connect(EDBUS_SYSTEM)) {
		alert(_("Unable to connect to system bus. Is DBus daemon running?"));
		return;
	}

	status_bar->value(_("Connected to system bus"));
	connection_bus_type = CONNECTED_BUS_SYSTEM;
	list_bus_names(bus_connection, service_browser);
}

static void disconnect_cb(Fl_Widget*, void*) {
	connection_bus_type = CONNECTED_BUS_NONE;
	status_bar->value(_("Not connected"));
	service_browser->clear();
	method_browser->clear();

	if(bus_connection) bus_connection->disconnect();
}

static void about_cb(Fl_Widget*, void*) {
	message(PROGRAM_LABEL " v" PROGRAM_VERSION);
}

static void browser_cb(Fl_Browser *b, void*) {
	if(b->value() > 0)
		method_browser->introspect(b->text(b->value()), bus_connection);
}

static void save_cb(Fl_Widget*, void*) {
	save_as_cb(0, (void*)saved_content);
}

static void save_as_cb(Fl_Widget*, void *saved) {
	char *path = (char*)saved;

	if(!path)
		path = fl_file_chooser(_("Save shell content"), LOAD_SAVE_PATTERN, 0, 0);

	if(!path) return;

	if(script_editor->buffer()->savefile(path) != 0)
		alert(_("Failed to save file as '%s'"), path);
	else
		saved_content = (const char*)path;
}

static void load_cb(Fl_Widget*, void*) {
	char *path = fl_file_chooser(_("Load shell content"), LOAD_SAVE_PATTERN, 0, 0);
	if(path && script_editor->buffer()->loadfile(path) != 0)
		alert(_("Failed to load file from '%s'"), path);
}

static void editor_undo_cb(Fl_Widget*, void*) {
	script_editor->undo_content();
}

static void editor_copy_cb(Fl_Widget*, void*) {
	script_editor->copy_content();
}

static void editor_paste_cb(Fl_Widget*, void*) {
	script_editor->paste_content();
}

static void editor_select_all_cb(Fl_Widget*, void*) {
	script_editor->select_all();
}

static void evaluate_cb(Fl_Widget*, void*) {
	script_editor->eval_selection(false);
}

static void evaluate_and_print_cb(Fl_Widget*, void*) {
	script_editor->eval_selection(true);
}

static void macro_expand_cb(Fl_Widget*, void*) {
	script_editor->eval_selection(true, true);
}

int main(int argc, char **argv) {
	EDE_APPLICATION("edelib-dbus-explorer");

	bus_connection = NULL;
	saved_content = NULL;

	win = new AppWindow(600, 505, PROGRAM_LABEL);
	win->begin();
		MenuBar *menu_bar = new MenuBar(0, 0, 600, 25);
		menu_bar->menu(menu_);

		Fl_Tile* tile = new Fl_Tile(5, 30, 590, 445);
		tile->begin();
			service_browser = new Fl_Hold_Browser(5, 50, 220, 320, _("Services"));
        	service_browser->align(Fl_Align(FL_ALIGN_TOP_LEFT));
			service_browser->callback((Fl_Callback*)browser_cb);

			method_browser = new ObjectTree(225, 50, 370, 320, _("Objects and methods"));
			method_browser->align(Fl_Align(FL_ALIGN_TOP_LEFT));

			script_editor = new ScriptEditor(5, 370, 590, 105);
			script_editor->init_scripting(&bus_connection);

			/* so method_browser can write in it */
			method_browser->set_editor_buffer(script_editor->buffer());
		tile->end();
		Fl_Group::current()->resizable(tile);

		status_bar = new Fl_Output(0, 480, 600, 25);
		status_bar->color(win->color());
	win->end();

	/* initialize state to none */
	disconnect_cb(0, 0);
	win->callback(quit_cb);
	win->show(argc, argv);

	int ret = Fl::run();
	delete bus_connection;
	return ret;
}
