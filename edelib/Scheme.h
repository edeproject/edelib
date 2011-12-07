/*
 * $Id$
 *
 * Scheme interpeter
 * Copyright (c) 2005-2011 edelib authors
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

#ifdef __cplusplus
extern "C" {
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
 * \todo Complete docs.
 */

/**
 * \ingroup scheme
 * Initialize scheme interpreter and returns scheme object. If something fails (e.g. not enough memory, return NULL). 
 */
#define edelib_scheme_init scheme_init_new
  
/**
 * \ingroup scheme
 * Deinitialize and clear scheme interpeter object.
 */
#define edelib_scheme_deinit scheme_deinit

/* some straight usage */
#define edelib_scheme_load_file    scheme_load_file
#define edelib_scheme_load_string  scheme_load_string

#define edelib_scheme_set_external_data scheme_set_external_data
#define edelib_scheme_define            scheme_define
#define edelib_scheme_call              scheme_call

/* try to avoid vpr by using function like macros */
#define edelib_scheme_gensym(sc)                 (sc)->vptr->gensym(sc)
#define edelib_scheme_cons(sc, a, b)             (sc)->vptr->cons(sc, a, b)	 
#define edelib_scheme_immutable_cons(sc, a, b)   (sc)->vptr->immutable_cons(sc, a, b)
#define edelib_scheme_reserve_cells(sc, n)       (sc)->vptr->reserve_cells(sc, n)
#define edelib_scheme_mk_int(sc, num)            (sc)->vptr->mk_integer(sc, num)
#define edelib_scheme_mk_double(sc, num)         (sc)->vptr->mk_real(sc, num)
#define edelib_scheme_mk_symbol(sc, sym)         (sc)->vptr->mk_symbol(sc, sym)
#define edelib_scheme_mk_string(sc, str)         (sc)->vptr->mk_string(sc, str)
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
#define edelib_scheme_pair_car(sc, p)      (sc)->vptr->pair_car(p) 
#define edelib_scheme_pair_cdr(sc, p)      (sc)->vptr->pair_cdr(p) 
#define edelib_scheme_car_set(sc, p, q)      (sc)->vptr->set_car(p, q)
#define edelib_scheme_cdr_set(sc, p, q)      (sc)->vptr->set_cdr(p, q)
#define edelib_scheme_is_symbol(sc, p)       (sc)->vptr->is_symbol(p) 
#define edelib_scheme_symname(sc, p)         (sc)->vptr->symname(p)
#define edelib_scheme_is_syntax(sc, p)         (sc)->vptr->is_syntax(p)
#define edelib_scheme_syntaxname(sc, p)        (sc)->vptr->syntaxname(p) 
#define edelib_scheme_is_proc(sc, p)         (sc)->vptr->is_proc(p)
#define edelib_scheme_is_macro(sc, p)         (sc)->vptr->is_macro(p)
#define edelib_scheme_is_foreign(sc, p)        (sc)->vptr->is_foreign(p) 
#define edelib_scheme_closure_code(sc, p)         (sc)->vptr->closure_code(p)
#define edelib_scheme_closure_env(sc, p)         (sc)->vptr->closure_env(p)
  
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
  sc->vptr->scheme_define(sc, sc->gobal_env,                            \
                              sc->vptr->mk_symbol(sc, func_name),       \
                              sc->vptr->mk_foreign_func(sc, func_ptr))  

/**
 * \def EDELIB_SCHEME_DEFINE2
 * \ingroup scheme
 * Define new scheme function with documentation.
 */
#define EDELIB_SCHEME_DEFINE2(sc, func_ptr, func_name, doc)     \
 do {                                                           \
   scheme_load_string(sc, "(add-doc " #func_name "," #doc ")"); \
   EDELIB_SCHEME_DEFINE(sc, func_name, func_ptr);               \
 } while(0)

#define EDELIB_SCHEME_OBJECT(sc) ((sc)->vptr)

/**
 * \def EDELIB_SCHEME_IS_NIL
 * \ingroup scheme
 * Macro for checking if given variable nil (NULL in scheme parlance).
 */
#define EDELIB_SCHEME_IS_NIL(sc, var) ((var) == sc->NIL)
  
#ifdef __cplusplus
}
#endif
#endif
