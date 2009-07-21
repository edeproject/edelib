/*
 * $Id$
 *
 * Base defs for edelib
 * Copyright (c) 2005-2009 edelib authors
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __EDELIB_GLOBAL_H__
#define __EDELIB_GLOBAL_H__

/* Needed so we don't have to include necessary libraries.  */
#ifndef NULL
	#ifndef __cplusplus
		#define NULL ((void*)0)
	#else 
		#define NULL 0
	#endif
#endif

#define EDELIB_NS edelib
#define EDELIB_NS_BEGIN namespace EDELIB_NS {
#define EDELIB_NS_END   }

/**
 * \def EDELIB_NS_USE
 * \ingroup macros
 *
 * This macro expands on <i>using namespace edelib;</i> or any other name used as the main
 * edelib namespace name. This macro is also the preferred to use than above expression in case
 * if namespace support was disabled.
 *
 * On other hand, globally including everything from edelib namespace is not preferred either;
 * via this way all edelib symbols will be known, causing possible name collisions. Alternative
 * to this is to explicitly include desired names via EDELIB_NS_USING.
 *
 * \note Please note how all <i>EDELIB_NS_*</i> must not be ended with semicolon! Semicolon is 
 * already present in the macro and this is the only exception for edelib macros.
 */
#define EDELIB_NS_USE using namespace EDELIB_NS;

/**
 * \def EDELIB_NS_USING
 * \ingroup macros
 *
 * Bring this name to the global namespace. This is the preferred way since you explicitly add desired
 * name to the global namespace.
 */
#define EDELIB_NS_USING(n) using EDELIB_NS::n;

/**
 * \def EDELIB_NS_USING_AS
 * \ingroup macros
 *
 * Redefine the name from edelib namespace to the global one. In the case of possible name collisions
 * when edelib name is introduced globally, this macro should be used to rename it.
 */
#define EDELIB_NS_USING_AS(old_name, new_name) typedef EDELIB_NS::old_name new_name;

/**
 * \def E_EXPORT
 * \ingroup macro
 * Marks the given symbol publicly visible.
 */

/**
 * \def E_NO_EXPORT
 * \ingroup macro
 * Marks the given symbol publicly not visible.
 */
#if __GNUC__ >= 4
	#define E_EXPORT __attribute__ ((visibility("default")))
	#define E_NO_EXPORT __attribute__ ((visibility("hidden")))
#else
	#define E_EXPORT
	#define E_NO_EXPORT
#endif

#define EDELIB_API E_EXPORT
#define EDELIB_NO_API E_NO_EXPORT

#ifdef __GNUC__
	#define EDELIB_DEPRECATED __attribute__ ((deprecated))
#else
	#define EDELIB_DEPRECATED
#endif

#ifdef HAVE_EDELIB_BASE_CONFIG_H
#include "_conf.h"
#endif

#endif
