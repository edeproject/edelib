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
 * \todo Complete docs.
 */

/**
 * \ingroup scheme
 * Initialize scheme interpreter and returns scheme object. If something fails (e.g. not enough memory, return NULL). 
 */
scheme *edelib_scheme_init(void);
  
/**
 * \ingroup scheme
 * Deinitialize and clear scheme interpeter object.
 */
void edelib_scheme_deinit(scheme *s);

#define edelib_scheme_load_file    scheme_load_file
#define edelib_scheme_load_string  scheme_load_string

#define edelib_scheme_set_external_data scheme_set_external_data
#define edelib_scheme_define            scheme_define
#define edelib_scheme_call              scheme_call

#define edelib_scheme_cons              _cons
#define edelib_scheme_mk_integer        mk_integer
#define edelib_scheme_mk_real           mk_real
#define edelib_scheme_mk_real           mk_symbol
#define edelib_scheme_gensym            gensym
#define edelib_scheme_mk_string         mk_string
#define edelib_scheme_mk_counted_string mk_counted_string
#define edelib_scheme_mk_character      mk_character
#define edelib_scheme_mk_foreign_func   mk_foreign_func
#define edelib_scheme_putstr            putstr
 
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
  
#ifdef __cplusplus
}
#endif
#endif
