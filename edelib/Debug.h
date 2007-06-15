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
EDELIB_API void EDebug(const char* fmt, ...);
EDELIB_API void EWarning(const char* fmt, ...);
EDELIB_API void EFatal(const char* fmt, ...);

#ifdef _DEBUG
	#define EASSERT(expr) \
	((expr) ? (void)0 : (EFatal("Assertion failed: \"%s\" in %s (%d)\n", #expr, __FILE__, __LINE__)))
#else
	#define EASSERT(expr) ((void)0)
#endif

#define EDEBUG   EDebug
#define EWARNING EWarning
#define EFATAL   EFatal

// current localtion of code (an cool idea from glib)
#define ESTRLOC_STRINGIFY(arg)          ESTRLOC_STRINGIFY_ARG(arg)
#define ESTRLOC_STRINGIFY_ARG(content)  #content

#define ESTRLOC __FILE__ ":" ESTRLOC_STRINGIFY(__LINE__)


EDELIB_NAMESPACE {

enum MsgType {
	MsgDebug = 0,         ///< debug messages
	MsgWarn,              ///< warnings
	MsgFatal              ///< failed assertions and EFatal calls
};

/** Handler type for error messages */
typedef void (*MsgHandlerType)(MsgType t, const char* msg);
/** 
 * Installs handler for error messages.
 *
 * This function is used to redirect all
 * messages to users defined function. User later
 * can do what is apropriate. This is sample
 * of usage:
 * \code
 *   MsgHandlerType myfunc;
 *   void myfunc(MsgType type, const char* msg)
 *   {
 *      switch(type)
 *      {
 *         // debug messages
 *         case MsgDebug:
 *            // do something
 *         break;
 *         // warrnings
 *         case MsgWarn:
 *            // do something
 *         break;
 *         // fatals and assertions (abort() is advised)
 *         case MsgFatal:
 *            abort();
 *         break;
 *       }
 *   }
 * \endcode
 */
EDELIB_API void InstallMsgHandler(MsgHandlerType m);

}
#endif
