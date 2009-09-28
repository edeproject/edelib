/*
 * $Id$
 *
 * Debug functions
 * Copyright (c) 2005-2007 edelib authors
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

#ifndef __EDELIB_DEBUG_H__
#define __EDELIB_DEBUG_H__

#include "edelib-global.h"

/**
 * \defgroup macros edelib macros
 */

/**
 * \def E_LOG_DOMAIN
 * \ingroup macros
 *
 * This macro should be set during compilation phase. It will name logging domain showing to
 * whom log output belongs.
 */
#ifndef E_LOG_DOMAIN
	#define E_LOG_DOMAIN ((char*)0)
#endif

/* these functions are not in the namespace so we don't get * strange output with the preprocessor */
EDELIB_API void _edelib_debug(const char* fmt, ...);
EDELIB_API void _edelib_warning(const char* fmt, ...);
EDELIB_API void _edelib_assert(int cond, const char* cond_text, const char* file, int line, const char* func);
EDELIB_API void _edelib_fatal(const char* fmt, ...);

#ifdef __GNUC__
	#define _E_FUNCTION_NAME __PRETTY_FUNCTION__
#else
	#define _E_FUNCTION_NAME "<unknown>"
#endif

/**
 * \def E_ASSERT
 * \ingroup macros
 *
 * Check if expression evaluates to false, in which case it will abort program, outputing offending
 * expression, file name and line number. If platform supports, it will try to output short stack content.
 */
#ifdef NDEBUG
	#define E_ASSERT(expr)
#else
	#define E_ASSERT(expr) _edelib_assert((expr) != 0, #expr, __FILE__, __LINE__, _E_FUNCTION_NAME)
#endif

/**
 * \def E_DEBUG
 * \ingroup macros
 *
 * Should be used for output debug information in stderr.
 */
#define E_DEBUG   _edelib_debug

/**
 * \def E_WARNING
 * \ingroup macros
 *
 * Should be use for output warnings in stderr.
 */
#define E_WARNING _edelib_warning

/**
 * \def E_FATAL
 * \ingroup macros
 *
 * Display error and call abort().
 */
#define E_FATAL   _edelib_fatal

#define _E_STRLOC_STRINGIFY(arg)          _E_STRLOC_STRINGIFY_ARG(arg)
#define _E_STRLOC_STRINGIFY_ARG(content)  #content

/**
 * \def E_STRLOC
 * \ingroup macros
 *
 * Stringify current location with file name and line number.
 */
#define E_STRLOC __FILE__ ":" _E_STRLOC_STRINGIFY(__LINE__)

/**
 * \def E_LIKELY
 * \ingroup macros
 *
 * E_LIKELY and E_UNLIKELY macros give a hint to compiler about expected results. 
 * Gcc can use these information for optimizations. These macros are inspired with G_LIKELY and G_UNLIKELY from glib.
 *
 * \note E_LIKELY(expr) is _not_ the same as !E_UNLIKELY(expr).
 */

/**
 * \def E_UNLIKELY
 * \ingroup macros
 *
 * \see E_LIKELY
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

/**
 * \def E_RETURN_IF_FAIL
 * \ingroup macros
 * 
 * E_RETURN_IF_FAIL is used for precondition and postcondition checks. When given
 * expression fails, macro will return from current function, outputting information
 * how condition failed.
 */
#define E_RETURN_IF_FAIL(expr)                                      \
	do {                                                            \
		if E_LIKELY(expr) { }                                       \
		else {                                                      \
			E_WARNING(E_STRLOC ": Condition '%s' failed\n", #expr); \
			return;                                                 \
		}                                                           \
	} while(0)                 

/**
 * \def E_RETURN_VAL_IF_FAIL
 * \ingroup macros
 *
 * Check if expression evaluates to the true (same as E_RETURN_IF_FAIL), but return
 * user specified value if expression fails.
 */
#define E_RETURN_VAL_IF_FAIL(expr, val)                             \
	do {                                                            \
		if E_LIKELY(expr) { }                                       \
		else {                                                      \
			E_WARNING(E_STRLOC ": Condition '%s' failed\n", #expr); \
			return (val);                                           \
		}                                                           \
	} while(0)


/* compatibility with the old code */
#define EDEBUG   E_DEBUG
#define EWARNING E_WARNING
#define EFATAL   E_FATAL
#define EASSERT  E_ASSERT
#define ESTRLOC  E_STRLOC

EDELIB_NS_BEGIN

/**
 * \enum ErrorMessageType
 * \brief Type of messages received in error message handler
 */
enum ErrorMessageType {
	ERROR_MESSAGE_DEBUG,      ///< Debug message
	ERROR_MESSAGE_WARNING,    ///< Warning message
	ERROR_MESSAGE_FATAL       ///< Fatal message
};

/** 
 * Installs handler for error messages
 */
EDELIB_API void error_mesage_handler_install(void (*)(ErrorMessageType t, const char* msg));

EDELIB_NS_END
#endif
