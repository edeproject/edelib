/*
 * $Id$
 *
 * Theming stuff
 * Copyright (c) 2009 edelib authors
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

#include <edelib/Theme.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <FL/Fl.H>
#include <edelib/Debug.h>
#include <edelib/Directory.h>
#include <edelib/Color.h>
#include <edelib/Resource.h>
#include <edelib/IconLoader.h>

#include "tinyscheme/scheme.h"
#include "tinyscheme/scheme-private.h"
#include "tinyscheme/init_scm.h"
#include "tinyscheme/theme_scm.h"

extern int FL_NORMAL_SIZE;

EDELIB_NS_BEGIN

/* 
 * If set to 1, when item in theme has function call (or anything that is pair type)
 * will be evaluated first and asigned as item value. E.g.
 *
 * (define (get-something)
 *  "something)
 *
 * (theme.style "foo"
 * [
 *  (xxx   (get-something) <= get-something will be seen as function here and will be evaluated
 * ])
 *
 * If not set, guess, it will not be evaluated, yielding value different than strint or long,
 * meaning 'xxx' will be seen as containing mallformed data.
 *
 * NOTE: These functions will not be evaluated when (theme.style-get) is called, simply because
 * it is stored as string symbol.
 */
#ifndef THEME_EVAL_VALUE_PAIR
# define THEME_EVAL_VALUE_PAIR 1
#endif

#define STORAGE_STR_SET(item, str) \
do {                               \
	if(item) free(item);           \
	item = strdup(str);            \
} while(0)

/* data that needs to be persistent */
struct StyleStorage {
	char *font_normal;
	char *font_bold;
	char *font_italic;

	StyleStorage() : font_normal(NULL), font_bold(NULL), font_italic(NULL) { }

	~StyleStorage() {
		if(font_normal) free(font_normal);
		if(font_bold)   free(font_bold);
		if(font_italic) free(font_italic);
	}
};

struct Theme_P {
	scheme     *sc;

	/* some metadata */
	const char *author;
	const char *name;
	const char *sample;

	/* evaluate scheme function successfully only once */
	pointer cached_ret;

	StyleStorage *storage;
};

/*
 * dirname()-like implementation. String must be copied first as real dirname() requires.
 * NOTE: it does not implements the case: dirname("/foo/") == "/".
 */
static char *local_dirname(char *path) {
	char *loc = strrchr(path, E_DIR_SEPARATOR);
	if(!loc)
		return path;

	/* Dump ending slashes. GNU dirname does it. */
	while(*loc == E_DIR_SEPARATOR && loc != path)
		loc--;

	*(loc + 1) = '\0';
	return path;
}

static void theme_p_init(Theme_P *t) {
	t->sc = NULL;
	t->author = t->name = t->sample = NULL;
	t->cached_ret = NULL;
	t->storage = NULL;
}

static StyleStorage *theme_p_get_storage(Theme_P *t) {
	if(!t->storage)
		t->storage = new StyleStorage;
	return t->storage;
}

static char *get_string_var(scheme *sc, const char *symbol) {
	pointer x = scheme_eval(sc, mk_symbol(sc, symbol));

	if(x != sc->NIL && sc->vptr->is_string(x))
		return sc->vptr->string_value(x);

	return NULL;
}

/* Call '(theme.style-get style_name)' from the script. Tinyscheme does not provide the better way... */
static pointer get_style_once(Theme_P *priv, scheme *sc, const char *style_name) {
	if(priv->cached_ret)
		return priv->cached_ret;

	pointer ret = scheme_eval(sc, 
			_cons(sc, mk_symbol(sc, "theme.style-get"), 
				_cons(sc, mk_string(sc, style_name), sc->NIL, 0), 0));

	/* allow to call this function again if someting went wrong */
	if(ret == sc->NIL || !sc->vptr->is_pair(ret))
		return sc->NIL;

	priv->cached_ret = ret;
	return ret;
}

Theme::Theme() : priv(NULL) {
	priv = new Theme_P;
	theme_p_init(priv);
}

Theme::~Theme() {
	clear();
	delete priv;
}

void Theme::clear(void) {
	if(!priv)
		return;

	if(priv->sc) {
		scheme_deinit(priv->sc);
		free(priv->sc);
	}

	delete priv->storage;
	theme_p_init(priv);
}

bool Theme::load(const char *name, const char *prefix) {
	return false;
}

bool Theme::load_from_file(const char *f) {
	E_RETURN_VAL_IF_FAIL(f != NULL, false);
	/* do not allow loading if claer() wasn't called before */
	E_RETURN_VAL_IF_FAIL(priv->sc == NULL, false);

	scheme *ss = scheme_init_new();
	if(!ss)
		return false;

	/* must be called */
	scheme_set_input_port_file(ss, stdin);
	scheme_set_output_port_file(ss, stdout);

	/* load init stuff */
	scheme_load_string(ss, init_scm_content);

	/* load theme stuff */
	scheme_load_string(ss, theme_scm_content);

	pointer sym;

	/* 
	 * Set (or override) common variables before actual script was loaded. 
	 * Variables are static and can't be changed.
	 */
	sym = mk_symbol(ss, "edelib-dir-separator");
	scheme_define(ss, ss->global_env, sym, mk_string(ss, E_DIR_SEPARATOR_STR));
	ss->vptr->setimmutable(sym);

	/* 
	 * Determine from which directory we loads file, and set that file as base directory
	 * where '(include)' statement can search additional files. Include uses 'private::theme.search-path'.
	 */
	char *path = strdup(f);
	if(!path)
		E_FATAL(E_STRLOC ": No memory\n");

	char *dir = local_dirname(path);

	/* If returned name is the same as file, dirname wasn't found directory name in given path. */
	if(strcmp(dir, f) != 0) {
		sym = mk_symbol(ss, "private:theme.search-path");
		scheme_define(ss, ss->global_env, sym, mk_string(ss, dir));
		ss->vptr->setimmutable(sym);
	}

	/* scheme copies path, so we do not need it any more */
	free(path);

	FILE *fd = fopen(f, "r");
	if(!fd) {
		scheme_deinit(ss);
		free(ss);
		return false;
	}

	priv->sc = ss;

	scheme_load_file(ss, fd);
	fclose(fd);

	if(ss->no_memory) {
		E_WARNING(E_STRLOC ": No memory to load theme source in interpreter\n");
		return false;
	}
		
	if(ss->retcode != 0)
		return false;

	/* fetch common variables */
	priv->name   = get_string_var(ss, "private:theme.name");
	priv->author = get_string_var(ss, "private:theme.author");
	priv->sample = get_string_var(ss, "private:theme.sample");
	return true;
}

bool Theme::loaded(void) const {
	E_RETURN_VAL_IF_FAIL(priv->sc != NULL, false);
	scheme *ss = priv->sc;

	return (!ss->no_memory && (ss->retcode == 0));
}

bool Theme::get_item(const char *style_name, const char *item_name, char *ret, unsigned int sz) {
	E_RETURN_VAL_IF_FAIL(priv->sc != NULL, false);
	E_RETURN_VAL_IF_FAIL(style_name != NULL, false);
	E_RETURN_VAL_IF_FAIL(item_name != NULL, false);
	E_RETURN_VAL_IF_FAIL(ret!= NULL, false);
	E_RETURN_VAL_IF_FAIL(sz > 0, false);

	scheme *ss = priv->sc;

	pointer style = get_style_once(priv, ss, style_name);
	if(style == ss->NIL)
		return false;

	pointer lst, item, val;
	char *ret_str = NULL;

	/* 
	 * 'style' has the following list: '((item value) (item value)...)'
	 * First item is a symbol and the second is string. Although tinyscheme displays them in the same way,
	 * they are different types and appropriate functions must be called during conversion.
	 */
	while(style != ss->NIL) {
		lst = ss->vptr->pair_car(style);

		item = ss->vptr->pair_car(lst);
		val = ss->vptr->pair_car(ss->vptr->pair_cdr(lst));

		/* check if we found matching item */
		if(ss->vptr->is_symbol(item) && strcmp(item_name, ss->vptr->symname(item)) == 0) {
#if THEME_EVAL_VALUE_PAIR
			/* 
			 * Now, see if item value is function; if is true, evaluate it and return the result.
			 * In this case, result must be a string.
			 */
			if(ss->vptr->is_pair(val))
				val = scheme_eval(ss, val);
#endif
			/* if it is not string, simply continue until find item with the string value */
			if(ss->vptr->string_value(val)) {
				ret_str = ss->vptr->string_value(val);
				break;
			}
		}

		style = ss->vptr->pair_cdr(style);
	}

	if(ret_str) {
		strncpy(ret, ret_str, sz);
		if(sz > 0)
			ret[sz - 1] = '\0';
	}

	return (ret_str != NULL);
}

bool Theme::get_item(const char *style_name, const char *item_name, long &ret, long fallback) {
	E_RETURN_VAL_IF_FAIL(priv->sc != NULL, false);
	E_RETURN_VAL_IF_FAIL(style_name != NULL, false);
	E_RETURN_VAL_IF_FAIL(item_name != NULL, false);

	scheme *ss = priv->sc;
	bool found = false;

	pointer style = get_style_once(priv, ss, style_name);
	if(style == ss->NIL) {
		ret = fallback;
		return found;
	}

	pointer lst, item, val;

	while(style != ss->NIL) {
		lst = ss->vptr->pair_car(style);

		item = ss->vptr->pair_car(lst);
		val = ss->vptr->pair_car(ss->vptr->pair_cdr(lst));

		/* check if we found matching item */
		if(ss->vptr->is_symbol(item) && strcmp(item_name, ss->vptr->symname(item)) == 0) {
#if THEME_EVAL_VALUE_PAIR
			if(ss->vptr->is_pair(val))
				val = scheme_eval(ss, val);
#endif
			/* if it is not long, simply continue until find item with the long value */
			if(ss->vptr->ivalue(val)) {
				ret = ss->vptr->ivalue(val);
				found = true;
				break;
			}
		}

		style = ss->vptr->pair_cdr(style);
	}

	if(!found)
		ret = fallback;

	return found;
}

const char* Theme::author(void) const {
	return priv->author;
}

const char* Theme::name(void) const {
	return priv->name;
}

const char* Theme::sample_image(void) const {
	return priv->sample;
}

void Theme::apply_common_gui_elements(void) {
	E_RETURN_IF_FAIL(priv->sc != NULL);

	char buf[128];
	unsigned char r, g, b;
	long font_sz;

	if(get_item("ede", "scheme", buf, sizeof(buf)))
		Fl::scheme(buf);

	if(get_item("ede", "background_color", buf, sizeof(buf))) {
		color_html_to_rgb(buf, r, g, b);
		Fl::background(r, g, b);
	}

	if(get_item("ede", "background_color2", buf, sizeof(buf))) {
		color_html_to_rgb(buf, r, g, b);
		Fl::background2(r, g, b);
	}

	if(get_item("ede", "foreground_color", buf, sizeof(buf))) {
		color_html_to_rgb(buf, r, g, b);
		Fl::foreground(r, g, b);
	}

	if(get_item("ede", "icon_theme", buf, sizeof(buf))) {
		if(IconLoader::inited())
			IconLoader::reload(buf);
	}

	/* TODO: document this: ignored if font_(normal|bold|italic) has given size */
	if(get_item("ede", "font_size", font_sz, 0)) {
		FL_NORMAL_SIZE = (int)font_sz;
		Fl::redraw();
	}

	if(get_item("ede", "font_normal", buf, sizeof(buf))) {
		StyleStorage *st = theme_p_get_storage(priv);
		STORAGE_STR_SET(st->font_normal, buf);

		Fl::set_font(FL_HELVETICA, st->font_normal);
	}

	if(get_item("ede", "font_bold", buf, sizeof(buf))) {
		StyleStorage *st = theme_p_get_storage(priv);
		STORAGE_STR_SET(st->font_bold, buf);

		Fl::set_font(FL_HELVETICA_BOLD, st->font_bold);
	}

	if(get_item("ede", "font_italic", buf, sizeof(buf))) {
		StyleStorage *st = theme_p_get_storage(priv);
		STORAGE_STR_SET(st->font_italic, buf);

		Fl::set_font(FL_HELVETICA_ITALIC, st->font_italic);
	}
}

E_CLASS_GLOBAL_EXPLICIT_IMPLEMENT(Theme)

EDELIB_NS_END
