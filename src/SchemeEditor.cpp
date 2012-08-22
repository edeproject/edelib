/*
 * $Id: String.h 2839 2009-09-28 11:36:20Z karijes $
 *
 * Fl_Text_Editor enhacement for scheme editing
 * Copyright (c) 2005-2012 edelib authors
 * 
 * This program is described in Chapter 4 of the FLTK Programmer's Guide.
 * Copyright 1998-2010 by Bill Spitzak and others.
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

#include <edelib/SchemeEditor.h>
#include <edelib/Debug.h>
#include <edelib/Color.h>
#include <FL/Fl.H>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define EDITOR_PARENS_OPEN  "([{"
#define EDITOR_PARENS_CLOSE ")]}"

EDELIB_NS_BEGIN

static Fl_Text_Display::Style_Table_Entry styletable[] = {     // Style table
	{ FL_BLACK,      FL_COURIER,        FL_NORMAL_SIZE }, // A - Plain
	{ FL_DARK_GREEN, FL_COURIER,        FL_NORMAL_SIZE }, // B - Line comments
	{ FL_DARK_GREEN, FL_COURIER_ITALIC, FL_NORMAL_SIZE }, // C - Block comments
	{ FL_BLUE,       FL_COURIER,        FL_NORMAL_SIZE }, // D - Strings
	{ FL_DARK_RED,   FL_COURIER,        FL_NORMAL_SIZE }, // E - Directives
	{ FL_DARK_RED,   FL_COURIER_BOLD,   FL_NORMAL_SIZE }, // F - Types
	{ FL_BLUE,       FL_COURIER_BOLD,   FL_NORMAL_SIZE }, // G - Keywords
	{ FL_DARK_GREEN, FL_COURIER,        FL_NORMAL_SIZE }, // H - Parenthesis
	{ FL_DARK_RED,   FL_COURIER_BOLD,   FL_NORMAL_SIZE }  // I - Highlighted Parenthesis
};

static const char *code_keywords[] = {
	"->>",
	"->",
	":",
	"abs",
	"acons",
	"add-doc",
	"add-doc-generic",
	"add-macro-doc",
	"add-to-include-path",
	"add-var-doc",
	"and",
	"anyatom->string",
	"append",
	"apply",
	"assoc",
	"assq",
	"assv",
	"atom?",
	"atom->string",
	"begin",
	"boolean?",
	"caaaar",
	"caaadr",
	"caaar",
	"caadar",
	"caaddr",
	"caadr",
	"caar",
	"cadaar",
	"cadadr",
	"cadar",
	"caddar",
	"cadddr",
	"caddr",
	"cadr",
	"call/cc",
	"call-with-current-continuation",
	"call-with-input-file",
	"call-with-output-file",
	"car",
	"case",
	"catch",
	"cdaaar",
	"cdaadr",
	"cdaar",
	"cdadar",
	"cdaddr",
	"cdadr",
	"cdar",
	"cddaar",
	"cddadr",
	"cddar",
	"cdddar",
	"cddddr",
	"cdddr",
	"cddr",
	"cdr",
	"cdrs",
	"char<=?",
	"char<?",
	"char=?",
	"char>=?",
	"char>?",
	"char?",
	"char-alphabetic?",
	"char-ci<=?",
	"char-ci<?",
	"char-ci=?",
	"char-ci>=?",
	"char-ci>?",
	"char-ci-cmp?",
	"char-cmp?",
	"char-downcase",
	"char->integer",
	"char-lower-case?",
	"char-numeric?",
	"char-ready?",
	"char-upcase",
	"char-upper-case?",
	"char-whitespace?",
	"close-input-port",
	"close-output-port",
	"close-port",
	"closure?",
	"cmp",
	"*colon-hook*",
	"complex?",
	"compose",
	"cond",
	"cond-action-list",
	"cond-eval",
	"cond-eval-or",
	"cond-expand-runtime",
	"condition",
	"cond-list",
	"cons",
	"cons-stream",
	"current-environment",
	"current-input-port",
	"current-output-port",
	"define",
	"defined?",
	"define-macro",
	"define-with-return",
	"defun",
	"defvar",
	"delay",
	"display",
	"do",
	"doc",
	"drop",
	"*edelib-dir-separator*",
	"edelib-scheme-objects",
	"*edelib-scheme-precedence-table-max*",
	"*edelib-version*",
	"else",
	"empty?",
	"environment?",
	"eof-object?",
	"eq?",
	"equal?",
	"eqv?",
	"error",
	"*error-hook*",
	"eval",
	"eval-polymorphic",
	"even?",
	"exact?",
	"exact->inexact",
	"exit",
	"expr",
	"fill-string!",
	"filter",
	"find-doc",
	"first",
	"flatten",
	"fold-left",
	"foldr",
	"fold-right",
	"for-each",
	"for",
	"force",
	"forced",
	"full-path",
	"gc",
	"gcd",
	"gc-verbose",
	"generic-assoc",
	"generic-member",
	"gensym",
	"get-closure-code",
	"if=",
	"if",
	"if-not=",
	"if-not",
	"in",
	"include",
	"*include-path*",
	"inexact?",
	"infix->prefix",
	"input-output-port?",
	"input-port?",
	"integer?",
	"integer->char",
	"interaction-environment",
	"lambda",
	"lazy",
	"length",
	"let",
	"let*",
	"let1",
	"let2",
	"let3",
	"let4",
	"let5",
	"letrec",
	"list?",
	"list",
	"list*",
	"list-ref",
	"list->string",
	"list-tail",
	"list->vector",
	"load",
	"loop",
	"macro?",
	"macro-expand",
	"make-closure",
	"make-environment",
	"make-string",
	"make-vector",
	"map",
	"max",
	"member",
	"memoize",
	"memq",
	"memv",
	"min",
	"mod",
	"modulo",
	"negative?",
	"newline",
	"newstr",
	"not",
	"nth",
	"null?",
	"number?",
	"number->string",
	"oblist",
	"odd?",
	"open-input-file",
	"open-input-output-file",
	"open-input-output-string",
	"open-input-string",
	"open-output-file",
	"open-output-string",
	"or",
	"output-port?",
	"pair?",
	"partition",
	"peek-char",
	"pop-handler",
	"port?",
	"positive?",
	"prec",
	"pred",
	"prev-inport",
	"prev-outport",
	"print",
	"println",
	"procedure?",
	"put",
	"quit",
	"quote",
	"quotient",
	"random-next",
	"range",
	"rational?",
	"read",
	"read-char",
	"real?",
	"remainder",
	"remove-from-include-path",
	"replace-all",
	"rest",
	"return",
	"reverse",
	"set!",
	"set-car!",
	"set-cdr!",
	"set-input-port",
	"set-output-port",
	"*sharp-hook*",
	"sort",
	"sort-vector",
	"sort-with-operators",
	"split-at",
	"string<=?",
	"string<?",
	"string=?",
	"string>=?",
	"string>?",
	"string?",
	"string",
	"string->anyatom",
	"string-append",
	"string->atom",
	"string-ci<=?",
	"string-ci<?",
	"string-ci=?",
	"string-ci>=?",
	"string-ci>?",
	"string-cmp?",
	"string-copy",
	"string-fill!",
	"string-length",
	"string->list",
	"string->number",
	"string-ref",
	"string-set!",
	"string->symbol",
	"substring",
	"succ",
	"symbol?",
	"symbol->string",
	"take",
	"throw",
	"unless",
	"unquote",
	"value",
	"vector?",
	"vector",
	"vector-equal?",
	"vector-fill!",
	"vector-length",
	"vector->list",
	"vector-ref",
	"vector-set!",
	"when",
	"when-not",
	"while",
	"with-input-from-file",
	"with-input-output-from-to-files",
	"with-output-to-file",
	"write",
	"write-char",
	"zero?",
};

const char *code_types[] = {
	">",
	"<",
	">=",
	"<=",
	"=",
	"!=",
	"+",
	"-",
	"*",
	"/"
	"<>"
};

extern "C" {
static int compare_keywords(const void *a, const void *b) { return (strcmp(*((const char **)a), *((const char **)b))); }
}

//
// 'style_parse()' - Parse text and produce style data.
//

static void
style_parse(const char *text, char *style, int length) {
  char		 current;
  int		 col;
  int		 last;
  char		 buf[255],
			 *bufptr;
  const char *temp;

  // Style letters:
  //
  // A - Plain
  // B - Line comments
  // C - Block comments
  // D - Strings
  // E - Directives
  // F - Types
  // G - Keywords

  for (current = *style, col = 0, last = 0; length > 0; length --, text ++) {
	if (current == 'B' || current == 'F' || current == 'G' || current == 'H') current = 'A';
	if (current == 'A') {
	  // Check for directives, comments, strings, and keywords...
	  if (col == 0 && *text == '#') {
		// Set style to directive
		current = 'E';
		//} else if (strncmp(text, ";;", 2) == 0) {
	  } else if (*text == ';') {
		current = 'B';
		for (; length > 0 && *text != '\n'; length --, text ++) *style++ = 'B';

		if (length == 0) break;
	  } else if (strncmp(text, "\\\"", 2) == 0) {
		// Quoted quote...
		*style++ = current;
		*style++ = current;
		text ++;
		length --;
		col += 2;
		continue;
	  } else if (*text == '\"') {
		current = 'D';
	  } else if (!last && (islower((*text)&255) || *text == '_')) {
		// Might be a keyword...
		for (temp = text, bufptr = buf;
			 (islower((*temp)&255) || *temp == '_') && bufptr < (buf + sizeof(buf) - 1);
			 *bufptr++ = *temp++);

		if (!islower((*temp)&255) && *temp != '_') {
		  *bufptr = '\0';

		  bufptr = buf;

		  if (bsearch(&bufptr, code_types,
					  sizeof(code_types) / sizeof(code_types[0]),
					  sizeof(code_types[0]), compare_keywords)) {
			while (text < temp) {
			  *style++ = 'F';
			  text ++;
			  length --;
			  col ++;
			}

			text --;
			length ++;
			last = 1;
			continue;
		  } else if (bsearch(&bufptr, code_keywords,
							 sizeof(code_keywords) / sizeof(code_keywords[0]),
							 sizeof(code_keywords[0]), compare_keywords)) {
			while (text < temp) {
			  *style++ = 'G';
			  text ++;
			  length --;
			  col ++;
			}

			text --;
			length ++;
			last = 1;
			continue;
		  }
		}
	  }
	} else if (current == 'D') {
	  // Continuing in string...
	  if (strncmp(text, "\\\"", 2) == 0) {
		// Quoted end quote...
		*style++ = current;
		*style++ = current;
		text ++;
		length --;
		col += 2;
		continue;
	  } else if (*text == '\"') {
		// End quote...
		*style++ = current;
		col ++;
		current = 'A';
		continue;
	  }
	}

	// Copy style info...
	if (current == 'A' && (strchr(EDITOR_PARENS_OPEN, *text) || strchr(EDITOR_PARENS_CLOSE, *text))) *style++ = 'H';
	else *style++ = current;
	col ++;

	last = isalnum((*text)&255) || *text == '_' || *text == '.';

	if (*text == '\n') {
	  // Reset column and possibly reset the style
	  col = 0;
	  if (current == 'B' || current == 'E') current = 'A';
	}
  }
}

static void style_init(Fl_Text_Buffer *textbuf, Fl_Text_Buffer *stylebuf) {
	char *style = new char[textbuf->length() + 1];
	char *text = textbuf->text();

	memset(style, 'A', textbuf->length());
	style[textbuf->length()] = '\0';

	if(!stylebuf) stylebuf = new Fl_Text_Buffer(textbuf->length());

	style_parse(text, style, textbuf->length());

	stylebuf->text(style);
	delete[] style;
	free(text);
}

static void
style_update(int		pos,			// I - Position of update
			 int		nInserted,		// I - Number of inserted chars
			 int		nDeleted,		// I - Number of deleted chars
			 int		/*nRestyled*/,	// I - Number of restyled chars
			 const char * /*deletedText*/,// I - Text that was deleted
			 void		*cbArg) {		// I - Callback data
  int	start,							// Start of text
		end;							// End of text
  char	last,							// Last style on line
		*style,							// Style data
		*text;							// Text data

  SchemeEditor *editor = (SchemeEditor*)cbArg;
  Fl_Text_Buffer *textbuf = editor->buffer();
  Fl_Text_Buffer *stylebuf = editor->style_buffer();

  // If this is just a selection change, just unselect the style buffer...
  if (nInserted == 0 && nDeleted == 0) {
	stylebuf->unselect();
	return;
  }

  // Track changes in the text buffer...
  if (nInserted > 0) {
	// Insert characters into the style buffer...
	style = new char[nInserted + 1];
	memset(style, 'A', nInserted);
	style[nInserted] = '\0';

	stylebuf->replace(pos, pos + nDeleted, style);
	delete[] style;
  } else {
	// Just delete characters in the style buffer...
	stylebuf->remove(pos, pos + nDeleted);
  }

  // Select the area that was just updated to avoid unnecessary
  // callbacks...
  stylebuf->select(pos, pos + nInserted - nDeleted);

  // Re-parse the changed region; we do this by parsing from the
  // beginning of the previous line of the changed region to the end of
  // the line of the changed region...	Then we check the last
  // style character and keep updating if we have a multi-line
  // comment character...
  start = textbuf->line_start(pos);
//	if (start > 0) start = textbuf->line_start(start - 1);
  end	= textbuf->line_end(pos + nInserted);
  text	= textbuf->text_range(start, end);
  style = stylebuf->text_range(start, end);
  if (start==end)
	last = 0;
  else
	last  = style[end - start - 1];

//	printf("start = %d, end = %d, text = \"%s\", style = \"%s\", last='%c'...\n",
//		   start, end, text, style, last);

  style_parse(text, style, end - start);

//	printf("new style = \"%s\", new last='%c'...\n",
//		   style, style[end - start - 1]);

  stylebuf->replace(start, end, style);
  editor->redisplay_range(start, end);

  if (start==end || last != style[end - start - 1]) {
//	  printf("Recalculate the rest of the buffer style\n");
	// Either the user deleted some text, or the last character
	// on the line changed styles, so reparse the
	// remainder of the buffer...
	free(text);
	free(style);

	end	  = textbuf->length();
	text  = textbuf->text_range(start, end);
	style = stylebuf->text_range(start, end);

	style_parse(text, style, end - start);

	stylebuf->replace(start, end, style);
	editor->redisplay_range(start, end);
  }

  free(text);
  free(style);
}

static void style_unfinished_cb(int, void*) { }

/* for indenting; inspired of indentation from FLUID */
static int indent_cb(int key, SchemeEditor *e) {
	if (e->buffer()->selected()) {
		e->insert_position(e->buffer()->primary_selection()->start());
		e->buffer()->remove_selection();
	}

	int pos, start;
	char *text, *ptr;

	pos = e->insert_position();
	start = e->line_start(pos);
	text = e->buffer()->text_range(start, pos);

	for (ptr = text; isspace(*ptr); ptr ++)
		;

	*ptr = '\0';  
	if (*text) {
		// use only a single 'insert' call to avoid redraw issues
		int n = strlen(text);
		char *b = (char*)malloc(n+2);
		*b = '\n';
		strcpy(b+1, text);
		e->insert(b);
		free(b);
	} else {
		e->insert("\n");
	}

	e->show_insert_position();
	e->set_changed();
	if(e->when() & FL_WHEN_CHANGED) e->do_callback();

	free(text);
	return 1;
}

SchemeEditor::SchemeEditor(int X, int Y, int W, int H, const char *l) : Fl_Text_Editor(X, Y, W, H, l) {
	pmatch = true;

	/* setup buffers */
	textbuf = new Fl_Text_Buffer();
	stylebuf = new Fl_Text_Buffer();

	buffer(textbuf);

	textbuf->add_modify_callback(style_update, this);
	textbuf->call_modify_callbacks();

	highlight_data(stylebuf, styletable,
				   sizeof(styletable) / sizeof(styletable[0]),
				   'A', style_unfinished_cb, 0);

	style_init(textbuf, stylebuf);
	add_key_binding(FL_Enter, FL_TEXT_EDITOR_ANY_STATE, (Fl_Text_Editor::Key_Func)indent_cb);

	selection_color(FL_GRAY - 9);
}

void SchemeEditor::highlight_parens(int s, int e) {
	buffer()->highlight(s, e);
}

void SchemeEditor::unhighlight_parens(void) {
	buffer()->unhighlight();
}

void SchemeEditor::textsize(int s) {
	unsigned int sz = sizeof(styletable) / sizeof(styletable[0]);
	for(unsigned int i = 0; i < sz; i++)
		styletable[i].size = s;
}

#if (FL_MAJOR_VERSION >= 1) && (FL_MINOR_VERSION >= 3)
# define BUFFER_CHAR_AT(buf, i) ((buf)->char_at(i))
#else
# define BUFFER_CHAR_AT(buf, i) ((buf)->character(i))
#endif

int SchemeEditor::handle(int e) {
	int ret = Fl_Text_Editor::handle(e);
	if(!pmatch) return ret;

	if(e == FL_KEYBOARD || e == FL_PUSH) {
		if(!buffer()) return ret;
		
		unhighlight_parens();

		int pos = insert_position() - 1;
		if(pos < 0) return ret;

		int ch = BUFFER_CHAR_AT(buffer(), pos);

		if(strchr(EDITOR_PARENS_CLOSE, ch)) {
			int deep = 0;

			/* go backward and search matching open one */
			for(int p = pos; p >= 0; p--) {
				if(strchr(EDITOR_PARENS_CLOSE, BUFFER_CHAR_AT(buffer(), p))) {
					deep++;
					continue;
				}

				if(strchr(EDITOR_PARENS_OPEN, BUFFER_CHAR_AT(buffer(), p))) {
					deep--;

					if(deep == 0) {
						highlight_parens(p, pos + 1);
						return ret;
					}
				}
			}
		}

		if(strchr(EDITOR_PARENS_OPEN, ch)) {
			int deep = 0;

			/* go forward and search matching one */
			for(int p = pos; p < buffer()->length(); p++) {
				if(strchr(EDITOR_PARENS_OPEN, BUFFER_CHAR_AT(buffer(), p))) {
					deep++;
					continue;
				}

				if(strchr(EDITOR_PARENS_CLOSE, BUFFER_CHAR_AT(buffer(), p))) {
					deep--;

					if(deep == 0) {
						highlight_parens(pos, p + 1);
						return ret;
					}
				}
			}
		}
	}

	return ret;
}

void SchemeEditor::object_color(int id, int c) {
	unsigned int sz = sizeof(styletable) / sizeof(styletable[0]);
	E_RETURN_IF_FAIL(sz > (unsigned int)id);

	styletable[id].color = (Fl_Color)c;
}

void SchemeEditor::object_color(int id, const char *c) {
	object_color(id, color_html_to_fltk(c));
}

EDELIB_NS_END
