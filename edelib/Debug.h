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
 * these functions are not in the namespace so we don't get
 * strange output with the preprocessor
 */
EDELIB_API void _edelib_debug(const char* fmt, ...);
EDELIB_API void _edelib_warning(const char* fmt, ...);
EDELIB_API void _edelib_assert(int cond, const char* cond_text, const char* file, int line, const char* func);
EDELIB_API void _edelib_fatal(const char* fmt, ...);

#ifdef __GNUC__
	#define _E_FUNCTION_NAME __PRETTY_FUNCTION__
#else
	#define _E_FUNCTION_NAME "<unknown>"
#endif

#ifdef _DEBUG
	#define E_ASSERT(expr) _edelib_assert((expr) != 0, #expr, __FILE__, __LINE__, _E_FUNCTION_NAME)
#else
	#define E_ASSERT(expr)
#endif

#define E_DEBUG   _edelib_debug
#define E_WARNING _edelib_warning
#define E_FATAL   _edelib_fatal

#define _E_STRLOC_STRINGIFY(arg)          _E_STRLOC_STRINGIFY_ARG(arg)
#define _E_STRLOC_STRINGIFY_ARG(content)  #content

#define E_STRLOC __FILE__ ":" _E_STRLOC_STRINGIFY(__LINE__)

/*
 * E_LIKELY and E_UNLIKELY macros give a hint to compiler about expected results. 
 * gcc can use these information for optimizations. These macros are inspired with 
 * G_LIKELY and G_UNLIKELY from glib.
 *
 * Note that E_LIKELY(expr) is _not_ the same as !E_UNLIKELY(expr).
 */
#if defined(__GNUC__) && (__GNUC__ > 2) && defined(__OPTIMIZE__)
	#define _E_BOOLEAN_EXPR(expr)         \
	__extension__ ({                      \
			int _edelib_boolean_var_;     \
			if(expr)                      \
				_edelib_boolean_var_ = 1; \
			else                          \
				_edelib_boolean_var_ = 0; \
			_edelib_boolean_var_;         \
	})

	#define E_LIKELY(expr)   (__builtin_expect(_E_BOOLEAN_EXPR(expr), 1))
	#define E_UNLIKELY(expr) (__builtin_expect(_E_BOOLEAN_EXPR(expr), 0))
#else
	#define E_LIKELY(expr)   (expr)
	#define E_UNLIKELY(expr) (expr)
#endif

/*
 * E_RETURN_IF_FAIL and E_RETURN_VAL_IF_FAIL macros provide convinient
 * precondition and postcondition checks
 */
#define E_RETURN_IF_FAIL(expr)                                      \
	do {                                                            \
		if E_LIKELY(expr) { }                                       \
		else {                                                      \
			E_WARNING(E_STRLOC ": Condition '%s' failed\n", #expr); \
			return;                                                 \
		}                                                           \
	} while(0)                 

#define E_RETURN_VAL_IF_FAIL(expr, val)                             \
	do {                                                            \
		if E_LIKELY(expr) { }                                       \
		else {                                                      \
			E_WARNING(E_STRLOC ": Condition '%s' failed\n", #expr); \
			return (val);                                           \
		}                                                           \
	} while(0)


// compatibility with the old code
#define EDEBUG   E_DEBUG
#define EWARNING E_WARNING
#define EFATAL   E_FATAL
#define EASSERT  E_ASSERT
#define ESTRLOC  E_STRLOC

EDELIB_NS_BEGIN

/**
 * \enum MsgType
 * \brief Type of messages received in error message handler
 */
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

EDELIB_NS_END
#endif // __DEBUG_H__
