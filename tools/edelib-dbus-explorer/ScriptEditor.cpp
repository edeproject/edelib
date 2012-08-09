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
#include "ScriptEditor.h"

EDELIB_NS_USING(SchemeEditor)

static char eval_buf[1024];

ScriptEditor::ScriptEditor(int X, int Y, int W, int H, const char *l) : SchemeEditor(X, Y, W, H, l), sc(NULL) {
	sc = edelib_scheme_init();
	textsize(12);

	edelib_scheme_set_input_port_file(sc, stdin);
	edelib_scheme_set_output_port_string(sc, eval_buf, eval_buf + sizeof(eval_buf));

	object_color(6, "#314e6c");
	object_color(1, "#826647");

#if FL_MAJOR_VERSION >= 1 && FL_MINOR_VERSION >= 3
	wrap_mode(Fl_Text_Display::WRAP_AT_BOUNDS, w());
#endif

	buffer()->append(
";; This is edelib script editor and interactive shell.\n"
";; Type some expression and press SHIFT-Enter to evaluate it;\n"
";; the result will be visible inside editor.\n");
}

ScriptEditor::~ScriptEditor() {
	edelib_scheme_deinit(sc);
}

void ScriptEditor::eval_selection(void) {
	char *copy = NULL;

	if(buffer()->selected()) {
		copy = buffer()->selection_text();
	} else {
		/* check if we got something highlighted from parenthesis match */
		copy = buffer()->highlight_text();
	}

	if(!copy) return;
		
	memset(eval_buf, 0, sizeof(eval_buf));
	edelib_scheme_set_output_port_string(sc, eval_buf, eval_buf + sizeof(eval_buf));

	edelib_scheme_load_string(sc, copy);
	free(copy);

	buffer()->append(eval_buf);
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

		if(Fl::event_ctrl() && Fl::event_key() == 'l') {
			buffer()->remove(0, buffer()->length());
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
