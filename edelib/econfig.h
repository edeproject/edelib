/*
 * $Id$
 *
 * Base defs for edelib
 * Part of edelib.
 * Copyright (c) 2005-2007 EDE Authors.
 *
 * This program is licenced under terms of the 
 * GNU General Public Licence version 2 or newer.
 * See COPYING for details.
 */

#ifndef __ECONFIG_H__
#define __ECONFIG_H__

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

#include "_conf.h"

#endif
