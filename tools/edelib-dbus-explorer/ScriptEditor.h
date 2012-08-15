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

#ifndef __EDELIB_DBUS_EXPLORER_SCRIPT_EDITOR_H__
#define __EDELIB_DBUS_EXPLORER_SCRIPT_EDITOR_H__

#include <edelib/SchemeEditor.h>
#include <edelib/Scheme.h>
#include <edelib/EdbusConnection.h>

/* REPL and edelib-script editor */
class ScriptEditor : public EDELIB_NS_PREPEND(SchemeEditor) {
private:
	scheme *sc;
	int    template_pos;
	void search_template_word_and_highlight(void);
public:
	ScriptEditor(int X, int Y, int W, int H, const char *l = 0);
	~ScriptEditor();
	void init_scripting(EDELIB_NS_PREPEND(EdbusConnection) **con);
	void undo_content(void);
	void copy_content(void);
	void paste_content(void);
	void select_all(void);

	void eval_selection(bool print = false, bool macroexpand = false);
	void indent_selection(void);
	virtual int handle(int e);
};

#endif
