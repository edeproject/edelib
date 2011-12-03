/*
 * $Id:$
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
 * \defgroup scheme scheme interpreter
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
 
/**
 * \def EDELIB_SCHEME_DEFINE
 * \ingroup scheme
 * Define new scheme function.
 */ 
#define EDELIB_SCHEME_DEFINE(sc, func_name, func_ptr)	                \
  sc->vptr->scheme_define(sc, sc->gobal_env,                            \
                              sc->vptr->mk_symbol(sc, func_name),       \
                              sc->vptr->mk_foreign_func(sc, func_ptr))  

/**
 * \def EDELIB_SCHEME_DEFINE2
 * \ingroup scheme
 * Define new scheme function with documentation.
 */
#define EDELIB_SCHEME_DEFINE2(sc, func_name, doc, func_ptr)     \
 do {                                                           \
   scheme_load_string(sc, "(add-doc " #func_name "," #doc ")"); \
   EDELIB_SCHEME_DEFINE(sc, func_name, func_ptr);               \
 } while(0)
  
#ifdef __cplusplus
}
#endif
#endif
