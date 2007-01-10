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

#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "econfig.h"

/*
 * they are not in namespace so we don't get
 * strange output with preprocessor
 */
void EDebug(const char* fmt, ...);
void EWarning(const char* fmt, ...);
void EFatal(const char* fmt, ...);

#ifdef _DEBUG
	#define EASSERT(expr) \
	((expr) ? (void)0 : (EFatal("Assertion failed: \"%s\" in %s (%d)\n", #expr, __FILE__, __LINE__)))
#else
	#define EASSERT(expr) ((void)0)
#endif

#define EDEBUG   EDebug
#define EWARNING EWarning
#define EFATAL   EFatal

EDELIB_NAMESPACE {

enum MsgType
{
	MsgDebug = 0,
	MsgWarn,
	MsgFatal
};

typedef void (*MsgHandlerType)(MsgType t, const char* msg);
void InstallMsgHandler(MsgHandlerType m);

}
#endif
