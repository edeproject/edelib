/*
 * $Id: String.h 2839 2009-09-28 11:36:20Z karijes $
 *
 * Fl_Text_Editor enhacement for scheme editing
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

#ifndef __EDELIB_SCHEME_EDITOR_H__
#define __EDELIB_SCHEME_EDITOR_H__

#include "edelib-global.h"

#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Text_Buffer.H>

EDELIB_NS_BEGIN

/**
 * \class SchemeEditor
 * \brief Editing component for Scheme language
 *
 * SchemeEditor is extension of Fl_Text_Editor, where is provided syntax higlighting
 * for Scheme-like languages (and the rest of LISP family) with parenthesis matching. It will highligh
 * known Scheme keywords and functions, plus some extensions from <i>edelib-script</i> (see <i>sslib/init-2.ss</i> for
 * those extensions).
 *
 * \note This is simple editor intended for small code snippets where full editor is not suitable.
 *
 * There are few caveats you should be aware of, if you are planning to use this component. First, you
 * should not explicitly set editor buffer (via <i>Fl_Text_Editor::buffer</i>) as SchemeEditor will set it
 * when object is constructed. This is needed so class can correctly setup style buffer too (for syntax highlight).
 * If you specify own buffer object, syntax highlight will not work.
 *
 * The second is how SchemeEditor will use <i>Fl_Text_Editor::add_modify_callback</i> internally to match parenthesis,
 * so if you overwrite it, editor will not highlight them any more.
 */
class EDELIB_API SchemeEditor : public Fl_Text_Editor {
private:
	Fl_Text_Buffer *stylebuf, *textbuf;
	bool pmatch;

	void highlight_parens(int start, int end);
	void unhighlight_parens(void);
public:
	/** Create widget at given position with given size. */
	SchemeEditor(int X, int Y, int W, int H, const char *l = 0);

	/** Set text size; applied on all styles. */
	void textsize(int sz);

	/** Event handler. */
	virtual int handle(int e);

	/** Return style buffer. */
	Fl_Text_Buffer *style_buffer(void) { return stylebuf; }

	/**
	 * Set color for text object. Object can be keyword, comment, parenthesis, etc. and is
	 * setup according to given list:
	 * 
	 * - 0 - plain text
	 * - 1 - comments
	 * - 2 - strings
	 * - 3 - directives
	 * - 4 - types
	 * - 5 - keywords
	 * - 6 - parenthesis
	 *
	 * For example, to draw comments in red color, you would use <i>object_color(2, FL_RED)</i>.
	 */
	void object_color(int id, int color);

	/** Set color using html color */
	void object_color(int id, const char *color);

	/** Match or highlight block of parenthesis. */
	void match_parenthesis(bool m) { pmatch = m; }
};

EDELIB_NS_END
#endif
