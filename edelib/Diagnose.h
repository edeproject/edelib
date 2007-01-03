/*
 * $Id$
 *
 * Part of edelib.
 * Copyright (c) 2005-2007 EDE Authors.
 *
 * This program is licenced under terms of the 
 * GNU General Public Licence version 2 or newer.
 * See COPYING for details.
 */

#ifndef __DIAGNOSE_H__
#define __DIAGNOSE_H__

#ifdef _DEBUG
	#include <assert.h>
	#define EASSERT assert
#else
	#define EASSERT(expr) ((void)0)
#endif

#endif
