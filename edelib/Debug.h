/*
 * $Id$
 *
 * Debug functions
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

#ifndef __EDELIB_DEBUG_H__
#define __EDELIB_DEBUG_H__

#include "edelib-global.h"
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup macros edelib macros
 */

#ifndef SKIP_DOCS
EDELIB_API void edelib_logv(const char *domain, int type, const char *fmt, va_list args);
EDELIB_API void edelib_log(const char *domain, int type, const char *fmt, ...);
#endif

/**
 * \enum EdelibErrorMessageType
 * \brief Type of messages received in error message handler
 */
typedef enum {
	EDELIB_ERROR_MESSAGE_DEBUG,      /**< Debug message   */
	EDELIB_ERROR_MESSAGE_WARNING,    /**< Warning message */
	EDELIB_ERROR_MESSAGE_FATAL       /**< Fatal message   */
} EdelibErrorMessageType;

/** 
 * Installs handler for error messages
 */
EDELIB_API void edelib_error_mesage_handler_install(void (*)(int t, const char* domain, const char* msg));

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

#ifdef __GNUC__
 #define _E_FUNCTION_NAME __PRETTY_FUNCTION__
#else
 #define _E_FUNCTION_NAME "<unknown>"
#endif

/**
 * \def E_DEBUG
 * \ingroup macros
 *
 * Should be used for output debug information in stderr.
 */

/**
 * \def E_WARNING
 * \ingroup macros
 *
 * Should be use for output warnings in stderr.
 */

/**
 * \def E_FATAL
 * \ingroup macros
 *
 * Display error and call abort().
 */

#ifdef HAVE_ISO_VARARGS
 #define E_DEBUG(...)    edelib_log(E_LOG_DOMAIN, EDELIB_ERROR_MESSAGE_DEBUG, __VA_ARGS__)
 #define E_WARNING(...)  edelib_log(E_LOG_DOMAIN, EDELIB_ERROR_MESSAGE_WARNING, __VA_ARGS__)
 #define E_FATAL(...)    edelib_log(E_LOG_DOMAIN, EDELIB_ERROR_MESSAGE_FATAL, __VA_ARGS__)
#elif defined(HAVE_GNUC_VARARGS)
 #define E_DEBUG(format...)    edelib_log(E_LOG_DOMAIN, EDELIB_ERROR_MESSAGE_DEBUG, format)
 #define E_WARNING(format...)  edelib_log(E_LOG_DOMAIN, EDELIB_ERROR_MESSAGE_WARNING, format)
 #define E_FATAL(format...)    edelib_log(E_LOG_DOMAIN, EDELIB_ERROR_MESSAGE_FATAL, format)
#else
 void E_DEBUG(const char *fmt, ...);
 void E_WARNING(const char *fmt, ...);
 void E_FATAL(const char *fmt, ...);
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
 #define E_ASSERT(expr) \
	do { \
		if(!(expr)) \
		edelib_log(E_LOG_DOMAIN, EDELIB_ERROR_MESSAGE_FATAL, "Assertion failed: \"%s\" in %s (%d), function: \"%s\"\n", \
			#expr, __FILE__, __LINE__, _E_FUNCTION_NAME); \
	} while(0);
#endif

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

#ifdef __cplusplus
}
#endif

#endif
