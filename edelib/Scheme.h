/*
 * $Id$
 *
 * Scheme interpeter
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

#ifndef __EDELIB_SCHEME_H__
#define __EDELIB_SCHEME_H__

#ifndef SKIP_DOCS
# include "ts/scheme.h"
# include "ts/scheme-private.h"  
#endif

/**
 * \defgroup scheme Scheme interpreter
 * This code provides Scheme interpreter and implements most of R5RS specification. The backend is based on
 * <a href="http://tinyscheme.sourceforge.net">tinyscheme</a>.
 *
 * If you are not familiar with Scheme, you can find numerous tutorials online about this language. Here are two, quite suitable
 * for begginers:
 * - <a href="http://www.ccs.neu.edu/home/dorai/t-y-scheme/t-y-scheme.html">Teach Yourself Scheme in Fixnum Days</a>
 * - <a href="http://www.shido.info/lisp/idx_scm_e.html">Yet Another Scheme Tutorial</a>
 *
 * To initialize interpreter you start with <i>edelib_scheme_init</i> or <i>edelib_scheme_init_raw</i>, which will return interpreter
 * object. As interpreter does not use any global state, you can safely run multiple interpreter instances in different threads.
 *
 * \todo Complete docs.
 */

/**
 * \ingroup scheme
 * Alias for internal scheme object.
 */
typedef scheme  edelib_scheme_t;

/**
 * \ingroup scheme
 * Alias for internal pointer object.
 */
typedef pointer edelib_scheme_pointer_t;

/**
 * \ingroup scheme
 * Initialize scheme interpreter and returns scheme object. By default, it will try to search bootstrap files in builtin path
 * (depends on <i>--prefix</i> when <b>configure</b> script was run). This can be overridden by setting <i>EDELIB_SCHEME_INIT</i>
 * environment variable with explicitly given bootstrap files and their absolute path. The paths should be separated with <b>:</b>
 * (just as PATH), e.g. <i>path/init.ss:path/init-2.ss:/path/other.ss</i>.
 */
scheme *edelib_scheme_init(void);

/**
 * \ingroup scheme
 * Initialize scheme interpeter, but will not load any code. Useful for explicitly loading desired bootstrap code.
 */
#define edelib_scheme_init_raw scheme_init_new
  
/**
 * \ingroup scheme
 * Deinitialize and clear scheme interpeter object.
 */
#define edelib_scheme_deinit scheme_deinit

/**
 * \ingroup scheme
 * Load scheme source from file.
 */
#define edelib_scheme_load_file scheme_load_file

/**
 * \ingroup scheme
 * Load scheme source from file, but provide filename for error report.
 */
#define edelib_scheme_load_named_file scheme_load_named_file

/**
 * \ingroup scheme
 * Load scheme source from string.
 */
#define edelib_scheme_load_string  scheme_load_string

/**
 * \ingroup scheme
 * Call a scheme function with arguments.
 */
#define edelib_scheme_eval scheme_eval

/**
 * \ingroup scheme
 * Call a scheme function without arguments.
 */
#define edelib_scheme_apply0 scheme_apply0

/**
 * \ingroup scheme
 * Set additional data (pointer) accessible from scheme function. You can get data with <em>edelib_scheme_get_external_data</em>
 */
#define edelib_scheme_set_external_data scheme_set_external_data

/**
 * \ingroup scheme
 * Get data set with <em>edelib_scheme_set_external_data</em>.
 */
#define edelib_scheme_get_external_data(sc) (sc)->ext_data

/**
 * \ingroup scheme
 * Defines a new function. You shoud use EDELIB_SCHEME_DEFINE or EDELIB_SCHEME_DEFINE2 instead.
 */
#define edelib_scheme_define scheme_define

/**
 * \ingroup scheme
 * Calls scheme function.
 */
#define edelib_scheme_call scheme_call

/**
 * \ingroup scheme
 * Set FILE object as input port.
 */
#define edelib_scheme_set_input_port_file scheme_set_input_port_file

/**
 * \ingroup scheme
 * Set FILE object as output port.
 */
#define edelib_scheme_set_output_port_file scheme_set_output_port_file

/**
 * \ingroup scheme
 * Set character array as input port. You should give pointer to the first element and pointer to the last.
 */
#define edelib_scheme_set_input_port_string scheme_set_input_port_string

/**
 * \ingroup scheme
 * Set character array as output port. You should give pointer to the first element and pointer to the last.
 */
#define edelib_scheme_set_output_port_string scheme_set_output_port_string 

/**
 * \ingroup scheme
 * Generate unique symbol on every invocation. This is mostly useful inside macros.
 */
#define edelib_scheme_gensym(sc) (sc)->vptr->gensym(sc)

/**
 * \ingroup scheme
 * Construct a new list by prepending <i>a</i> element in <i>b</i> list. Element can be any scheme object.
 */
#define edelib_scheme_cons(sc, a, b) _cons((sc), a, b, 0)	 

/**
 * \ingroup scheme
 * Same as <i>edelib_scheme_cons</i>, except immutable list is created.
 */
#define edelib_scheme_immutable_cons(sc, a, b) _cons((sc), a, b, 1)

/**
 * \ingroup scheme
 * Reserve given number of cells.
 */
#define edelib_scheme_reserve_cells(sc, n) (sc)->vptr->reserve_cells(sc, n)

/**
 * \ingroup scheme
 * Convert integer to scheme number.
 */
#define edelib_scheme_mk_int(sc, num) (sc)->vptr->mk_integer(sc, num)

/**
 * \ingroup scheme
 * Convert double to scheme number.
 */
#define edelib_scheme_mk_double(sc, num) (sc)->vptr->mk_real(sc, num)

/**
 * \ingroup scheme
 * Make scheme symbol.
 */
#define edelib_scheme_mk_symbol(sc, sym) (sc)->vptr->mk_symbol(sc, sym)

/**
 * \ingroup scheme
 * Convert C string to scheme string.
 */
#define edelib_scheme_mk_string(sc, str) (sc)->vptr->mk_string(sc, str)

/**
 * \ingroup scheme
 * Convert C string to counted scheme string.
 */
#define edelib_scheme_mk_string_counted(sc, str) (sc)->vptr->mk_counted_string(sc, str)
#define edelib_scheme_mk_character(sc, c)        (sc)->vptr->mk_character(sc, c)
#define edelib_scheme_mk_vector(sc, len)         (sc)->vptr->mk_vector(sc, len) 
#define edelib_scheme_mk_foreign_func(sc, func)  (sc)->vptr->mk_foreign_func(sc, func)
#define edelib_scheme_putstr(sc, str)            (sc)->vptr->putstr(sc, str)
#define edelib_scheme_putcharacter(sc, c)        (sc)->vptr->putcharacter(sc, c)

#define edelib_scheme_is_string(sc, p)    (sc)->vptr->is_string(p) 
#define edelib_scheme_string_value(sc, p) (sc)->vptr->string_value(p)
#define edelib_scheme_is_int(sc, p)       (sc)->vptr->is_number(p) 
#define edelib_scheme_int_value(sc, p)    (int)((sc)->vptr->ivalue(p))
#define edelib_scheme_is_double(sc, p)    (sc)->vptr->is_real(p)
#define edelib_scheme_double_value(sc, p) (sc)->vptr->rvalue(p)
#define edelib_scheme_is_char(sc, p)      (sc)->vptr->is_character(p)
#define edelib_scheme_char_value(sc, p)   (sc)->vptr->charvalue(p)
#define edelib_scheme_is_vector(sc, p)    (sc)->vptr->is_vector(p)
#define edelib_scheme_vector_len(sc, p)   (sc)->vptr->vector_length(p)
#define edelib_scheme_vector_fill(sc, vec, v) (sc)->vptr->fill_vector(vec, v)  
#define edelib_scheme_vector_elem(sc, vec, i) (sc)->vptr->vector_elem(vec, i)  
#define edelib_scheme_vector_elem_set(sc, vec, i, newel) (sc)->vptr->set_vector_elem(vec, i, newel) 
#define edelib_scheme_is_port(sc, p)      (sc)->vptr->is_port(p) 
#define edelib_scheme_is_pair(sc, p)      (sc)->vptr->is_pair(p) 
#define edelib_scheme_pair_car(sc, p)     (sc)->vptr->pair_car(p) 
#define edelib_scheme_pair_cdr(sc, p)     (sc)->vptr->pair_cdr(p) 
#define edelib_scheme_car_set(sc, p, q)   (sc)->vptr->set_car(p, q)
#define edelib_scheme_cdr_set(sc, p, q)   (sc)->vptr->set_cdr(p, q)
#define edelib_scheme_is_symbol(sc, p)    (sc)->vptr->is_symbol(p) 
#define edelib_scheme_symname(sc, p)      (sc)->vptr->symname(p)
#define edelib_scheme_is_syntax(sc, p)    (sc)->vptr->is_syntax(p)
#define edelib_scheme_syntaxname(sc, p)   (sc)->vptr->syntaxname(p) 
#define edelib_scheme_is_proc(sc, p)      (sc)->vptr->is_proc(p)
#define edelib_scheme_is_macro(sc, p)     (sc)->vptr->is_macro(p)
#define edelib_scheme_is_foreign(sc, p)   (sc)->vptr->is_foreign(p) 
#define edelib_scheme_closure_code(sc, p) (sc)->vptr->closure_code(p)
#define edelib_scheme_closure_env(sc, p)  (sc)->vptr->closure_env(p)
  
#define edelib_scheme_is_continuation(sc, p) (sc)->vptr->is_continuation(p)
#define edelib_scheme_is_promise(sc, p)      (sc)->vptr->is_promise(p)
#define edelib_scheme_is_environment(sc, p)  (sc)->vptr->is_environment(p)
  
#define edelib_scheme_is_immutable(sc, p)  (sc)->vptr->is_immutable(p)
#define edelib_scheme_set_immutable(sc, p) (sc)->vptr->setimmutable(p)
 
/**
 * \def EDELIB_SCHEME_DEFINE
 * \ingroup scheme
 * Define new scheme function.
 */ 
#define EDELIB_SCHEME_DEFINE(sc, func_ptr, func_name)	                \
  sc->vptr->scheme_define(sc, sc->global_env,                           \
                              sc->vptr->mk_symbol(sc, func_name),       \
                              sc->vptr->mk_foreign_func(sc, func_ptr))  

/**
 * \def EDELIB_SCHEME_DEFINE2
 * \ingroup scheme
 * Define new scheme function with documentation.
 */
#define EDELIB_SCHEME_DEFINE2(sc, func_ptr, func_name, doc)      \
  do {                                                           \
	scheme_load_string(sc, "(add-doc " #func_name " " #doc ")"); \
	EDELIB_SCHEME_DEFINE(sc, func_ptr, func_name);				 \
  } while(0)

#define EDELIB_SCHEME_OBJECT(sc)     ((sc)->vptr)
#define EDELIB_SCHEME_GLOBAL_ENV(sc) ((sc)->global_env)

/**
 * \def EDELIB_SCHEME_IS_NIL
 * \ingroup scheme
 * Macro for checking if given variable nil (NULL in scheme parlance).
 */
#define EDELIB_SCHEME_IS_NIL(sc, var) ((var) == sc->NIL)

#endif
