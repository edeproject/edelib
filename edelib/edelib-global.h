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

/*
 * Needed so we don't have to include
 * necessary libraries.
 */
#ifndef NULL
	#ifndef __cplusplus
		#define NULL ((void*)0)
	#else 
		#define NULL 0
	#endif
#endif

#define EDELIB_NS edelib
#define EDELIB_NS_USE using namespace EDELIB_NS;
#define EDELIB_NS_USING(n) using EDELIB_NS::n;
#define EDELIB_NS_BEGIN namespace EDELIB_NS {
#define EDELIB_NS_END   }

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
