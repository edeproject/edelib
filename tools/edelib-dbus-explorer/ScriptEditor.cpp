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

#include <string.h>
#include <stdlib.h>
#include <FL/Fl.H>
#include <edelib/EdbusConnection.h>
#include <edelib/Debug.h>
#include <edelib/Missing.h>
#include <edelib/String.h>

#include "ScriptEditor.h"
#include "ScriptDBus.h"
#include "Default.h"

EDELIB_NS_USING(SchemeEditor)
EDELIB_NS_USING(EdbusConnection)
EDELIB_NS_USING(String)

static char eval_buf[EDELIB_DBUS_EXPLORER_DEFAULT_SCRIPT_EVAL_BUFSIZE];

ScriptEditor::ScriptEditor(int X, int Y, int W, int H, const char *l) : SchemeEditor(X, Y, W, H, l), sc(NULL), template_pos(0), eval_result_insert(false) {
	textsize(12);
	selection_color(FL_GRAY - 9);

#if FL_MAJOR_VERSION >= 1 && FL_MINOR_VERSION >= 3
	wrap_mode(Fl_Text_Display::WRAP_AT_BOUNDS, w());
#endif
}

ScriptEditor::~ScriptEditor() {
	if(sc) edelib_scheme_deinit(sc);
}

void ScriptEditor::search_template_word_and_highlight(void) {
	if(!buffer()->search_forward(template_pos, EDELIB_DBUS_EXPLORER_DEFAULT_VALUE_TEMPLATE, &template_pos, 1)) {
		template_pos = 0;
		return;
	}

	int len = edelib_strnlen(EDELIB_DBUS_EXPLORER_DEFAULT_VALUE_TEMPLATE, 128);
	buffer()->select(template_pos, template_pos + len);

	template_pos += len;
}

void ScriptEditor::append_result(const char *r) {
	if(eval_result_insert) {
		Fl_Text_Editor::kf_move(FL_End, this);
		insert(r);
	} else {
		buffer()->append(r);
	}
}

void ScriptEditor::init_scripting(EdbusConnection **con) {
	E_RETURN_IF_FAIL(sc == NULL);

	sc = edelib_scheme_init();
	edelib_scheme_set_input_port_file(sc, stdin);
	edelib_scheme_set_output_port_string(sc, eval_buf, eval_buf + sizeof(eval_buf) - 1);

	/*
	 * init dbus binding
	 * NOTE: EdbusConnection here can be NULL, depending if connection was initialized or not
	 */
	script_dbus_load(sc, con);
	script_dbus_setup_help(sc, this);

	object_color(6, "#314e6c");
	object_color(1, "#826647");

	append_result(
";; This is edelib script editor and interactive shell.\n"
";; Type some expression and press SHIFT-Enter to evaluate it.\n"
";; To get help, type '(help)' and evaluate it with SHIFT-Enter.\n");

	/* append whatever was reported from above evaluation(s) */
	append_result(eval_buf);

	/* make all evaluation results inline */
	result_insert(true);
}

void ScriptEditor::undo_content(void) {
	Fl_Text_Editor::kf_undo(0, this);
}

void ScriptEditor::copy_content(void) {
	Fl_Text_Editor::kf_copy(0, this);
}

void ScriptEditor::paste_content(void) {
	Fl_Text_Editor::kf_paste(0, this);
}

void ScriptEditor::select_all(void) {
	Fl_Text_Editor::kf_select_all(0, this);
}

void ScriptEditor::eval_selection(bool print, bool macroexpand) {
	char *copy = NULL;

	if(buffer()->selected()) {
		copy = buffer()->selection_text();
	} else {
		/* check if we got something highlighted from parenthesis match */
		copy = buffer()->highlight_text();
	}

	if(!copy || edelib_strnlen(copy, EDELIB_DBUS_EXPLORER_DEFAULT_SCRIPT_EVAL_BUFSIZE) < 1)
		return;
		
	memset(eval_buf, 0, sizeof(eval_buf));
	edelib_scheme_set_output_port_string(sc, eval_buf, eval_buf + sizeof(eval_buf) - 1);

	if(print) {
		String buf = "(display ";

		/* made intentionally that macro expansion only works when 'print' is set to true */
		if(macroexpand)
			buf += "(macro-expand '";
		buf += copy;

		if(macroexpand)
			buf += ')';

		buf += ')';
		free(copy);

		edelib_scheme_load_string(sc, buf.c_str());

		/* tokenize thing so we can append comments */
		for(char *tok = strtok(eval_buf, "\n"); tok; tok = strtok(NULL, "\n")) {
			append_result("\n;; ");
			append_result(tok);
			append_result("\n");
		}
	} else {
		edelib_scheme_load_string(sc, copy);
		free(copy);
		append_result(eval_buf);
	}
}

void ScriptEditor::indent_selection(void) {
}

int ScriptEditor::handle(int e) {
	/* catch shortcut before we enter into SchemeEditor */
	if(e == FL_KEYBOARD) {
		if((Fl::event_ctrl() && Fl::event_key() == 'j') ||
		   (Fl::event_shift() && Fl::event_key(FL_Enter)))
		{
			eval_selection();
			return 1;
		}

		if(Fl::event_alt() && Fl::event_key(FL_Enter)) {
			eval_selection(true);
			return 1;
		}

		if(Fl::event_ctrl() && Fl::event_key() == 'l') {
			buffer()->remove(0, buffer()->length());
			return 1;
		} 

		if(Fl::event_shift() && Fl::event_key() == FL_Tab) {
			search_template_word_and_highlight();
			return 1;
		}

#if 0
		/* emacsy keys */
		if(Fl::event_alt() && Fl::event_key() == 'f') {
			next_word();
			return 1;
		}

		if(Fl::event_alt() && Fl::event_key() == 'b') {
			previous_word();
			return 1;
		}

		if(Fl::event_ctrl() && Fl::event_key() == 'a') {
			Fl_Text_Editor::kf_move(FL_Home, this);
			return 1;
		}
		
		if(Fl::event_ctrl() && Fl::event_key() == 'e') {
			Fl_Text_Editor::kf_move(FL_End, this);
			return 1;
		}

		if(Fl::event_ctrl() && Fl::event_key() == 'v') {
			Fl_Text_Editor::kf_move(FL_Page_Up, this);
			return 1;
		}

		if(Fl::event_alt() && Fl::event_key() == 'v') {
			Fl_Text_Editor::kf_move(FL_Page_Down, this);
			return 1;
		}
#endif
	}

	return SchemeEditor::handle(e);
}
