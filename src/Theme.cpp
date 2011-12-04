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
#include <limits.h>

#include <edelib/Debug.h>
#include <edelib/Directory.h>
#include <edelib/Missing.h>
#include <edelib/Version.h>
#include <edelib/Scheme.h>
#include "../sslib/init_ss.h"
#include "../sslib/theme_ss.h"

extern int FL_NORMAL_SIZE;

EDELIB_NS_BEGIN

/* 
 * If set to 1, when item in theme has function call (or anything that is pair type)
 * will be evaluated first and asigned as item value. E.g.
 *
 * (define (get-something)
 *  "something)
 *
 * (theme.style "foo"  [
 *    xxx (get-something) <= get-something will be seen as function here and will be evaluated
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

struct Theme_P {
	scheme     *sc;

	/* some metadata */
	const char *author;
	const char *name;
	const char *sample;

	/* evaluate scheme function successfully only once */
	pointer cached_ret;

	/* to make sure the script was loaded */
	bool is_loaded;

	/* error handler */
	ThemeErrorHandler err_func;
	void              *err_func_data;
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
	t->is_loaded = false;

	t->err_func = NULL;
	t->err_func_data = NULL;
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

static pointer theme_error_hook(scheme *ss, pointer args) {
	if(args == ss->NIL) return ss->F;

	Theme_P *t = (Theme_P*)ss->ext_data;
	E_ASSERT(t && "Theme_P hook installed without this pointer");

	if(!t->err_func) return ss->F;

	pointer arg, a = args;
	String buf;

	for(; a != ss->NIL; a = ss->vptr->pair_cdr(a)) {
		arg = ss->vptr->pair_car(a);

		if(ss->vptr->is_string(arg))
			buf.append(ss->vptr->string_value(arg));
		else if(ss->vptr->is_symbol(arg))
			buf.append(ss->vptr->symname(arg));
	}

	t->err_func(buf.c_str(), t->err_func_data);
	return ss->T;
}

Theme::Theme(const char *msg_domain) : priv(NULL) {
	priv = new Theme_P;
	theme_p_init(priv);
}

Theme::~Theme() {
	clear();
	delete priv;
}

void Theme::init_interpreter(void) {
	if(priv->sc) return;

	scheme *ss = edelib_scheme_init();
	if(!ss) {
		E_WARNING(E_STRLOC ": Unable to init interpreter\n");
		return;
	}

	priv->sc = ss;

	/* must be called */
	scheme_set_input_port_file(ss, stdin);
	scheme_set_output_port_file(ss, stdout);

	/* install user supplied error handler first, if given */
	if(priv->err_func) {
		ss->vptr->scheme_define(ss,
								ss->global_env,
								ss->vptr->mk_symbol(ss, "private:theme.error_hook"),
								ss->vptr->mk_foreign_func(ss, theme_error_hook));

		/* make sure interpreter does not use this function at all */
		scheme_set_external_data(ss, (void*)priv);
	}

	/* load init stuff */
	scheme_load_string(ss, init_ss_content);

	/* load theme stuff */
	scheme_load_string(ss, theme_ss_content);

	/* 
	 * Set (or override) common variables before actual script was loaded. 
	 * Variables are static and can't be changed.
	 */
	pointer sym;

	sym = mk_symbol(ss, "*edelib-dir-separator*");
	scheme_define(ss, ss->global_env, sym, mk_string(ss, E_DIR_SEPARATOR_STR));
	ss->vptr->setimmutable(sym);

	sym = mk_symbol(ss, "*edelib-version*");
	scheme_define(ss, ss->global_env, sym, mk_string(ss, EDELIB_VERSION));
	ss->vptr->setimmutable(sym);
}

bool Theme::load(const char *f) {
	E_RETURN_VAL_IF_FAIL(f != NULL, false);
	/* do not allow loading if clear() wasn't called before */
	E_RETURN_VAL_IF_FAIL(priv->sc == NULL, false);
	priv->is_loaded = false;

	init_interpreter();
	scheme *ss = priv->sc;

	/* 
	 * Determine from which directory we loads file, and set that file as base directory
	 * where '(include)' statement can search additional files. Include uses 'private::theme.search-path'.
	 */
	char *path = edelib_strndup(f, PATH_MAX);
	if(!path)
		E_FATAL(E_STRLOC ": No memory\n");

	char *dir = local_dirname(path);

	/* If returned name is the same as file, dirname wasn't found directory name in given path. */
	if(strcmp(dir, f) != 0) {
		pointer sym = mk_symbol(ss, "private:theme.search-path");
		scheme_define(ss, ss->global_env, sym, mk_string(ss, dir));
		ss->vptr->setimmutable(sym);
	}

	/* scheme copies path, so we do not need it any more */
	free(path);

	FILE *fd = fopen(f, "r");
	if(!fd) {
		edelib_scheme_deinit(ss);
		free(ss);
		priv->sc = 0;
		return false;
	}

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

	priv->is_loaded = true;
	return true;
}

void Theme::set_error_handler(ThemeErrorHandler f, void *data) {
	priv->err_func = f;
	priv->err_func_data = data;
}

ThemeErrorHandler Theme::error_handler(void) const {
	return priv->err_func;
}

void *Theme::error_handler_data(void) const {
	return priv->err_func_data;
}

void Theme::clear(void) {
	if(!priv)
		return;

	if(priv->sc) {
		edelib_scheme_deinit(priv->sc);
		free(priv->sc);
	}

	theme_p_init(priv);
}

bool Theme::loaded(void) const { 
	return priv->is_loaded; 
}

void Theme::prompt(void) {
	init_interpreter();

	printf("Theme console (edelib %s). Type \"(quit)\" to exit.", EDELIB_VERSION);
	scheme_load_file(priv->sc, stdin);
}

bool Theme::get_item(const char *style_name, const char *item_name, char *ret, unsigned int sz) {
	E_RETURN_VAL_IF_FAIL(priv->is_loaded == true, false);
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
			if(ss->vptr->is_string(val)) {
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
	E_RETURN_VAL_IF_FAIL(priv->is_loaded == true, false);
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

EDELIB_NS_END
