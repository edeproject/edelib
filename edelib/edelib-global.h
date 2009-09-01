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
# ifndef __cplusplus
#  define NULL ((void*)0)
# else 
#  define NULL 0
# endif
#endif

/**
 * \def EDELIB_NS
 * \ingroup macros
 * This is a name of the main namespace edelib uses.
 */
#ifndef EDELIB_NS
# define EDELIB_NS edelib
#endif

#ifdef EDELIB_NS
# define EDELIB_NS_BEGIN namespace EDELIB_NS {
# define EDELIB_NS_END   }
#else
# define EDELIB_NS_BEGIN
# define EDELIB_NS_END
#endif

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
#ifdef EDELIB_NS
# define EDELIB_NS_USE using namespace EDELIB_NS;
#else
# define EDELIB_NS_USE
#endif

/**
 * \def EDELIB_NS_USING
 * \ingroup macros
 *
 * Bring this name to the global namespace. This is the preferred way since you explicitly add desired
 * name to the global namespace.
 */
#ifdef EDELIB_NS
# define EDELIB_NS_USING(n) using EDELIB_NS::n;
#else
# define EDELIB_NS_USING(n)
#endif

/**
 * \def EDELIB_NS_USING_AS
 * \ingroup macros
 *
 * Redefine the name from edelib namespace to the global one. In the case of possible name collisions
 * when edelib name is introduced globally, this macro should be used to rename it.
 */
#ifdef EDELIB_NS
# define EDELIB_NS_USING_AS(old_name, new_name) typedef EDELIB_NS::old_name new_name;
#else
# define EDELIB_NS_USING_AS(old_name, new_name) typedef old_name new_name;
#endif

/**
 * \def EDELIB_NS_PREPEND
 * \ingroup macros
 * Prepend default edelib namespace to the given identifier.
 */
#ifdef EDELIB_NS
# define EDELIB_NS_PREPEND(n) EDELIB_NS::n
#else
# define EDELIB_NS_PREPEND(n) n
#endif

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
# define E_EXPORT __attribute__ ((visibility("default")))
# define E_NO_EXPORT __attribute__ ((visibility("hidden")))
#else
# define E_EXPORT
# define E_NO_EXPORT
#endif

#define EDELIB_API E_EXPORT
#define EDELIB_NO_API E_NO_EXPORT

/**
 * \def E_DISABLE_CLASS_COPY
 * \ingroup macros
 *
 * Disable class copying facility. Should be placed in private section
 * of class declaration.
 */
#define E_DISABLE_CLASS_COPY(klass) \
	klass(const klass&);            \
	klass& operator=(klass&);

/**
 * \def E_CLASS_GLOBAL_DECLARE
 * \ingroup macros
 *
 * Declare members to access single class instance. These classes has <em>global()</em> member and will always 
 * return one class instance. This macro us placed in class declaration.
 *
 * <em>E_CLASS_GLOBAL_DECLARE</em> and <em>E_CLASS_GLOBAL_IMPLEMENT</em> will create object on
 * stack. Object will be created when <em>global()</em> was called for the first time, and will
 * be destroyed when program quits.
 */
#define E_CLASS_GLOBAL_DECLARE(klass) \
	static klass* global(void);

/**
 * \def E_CLASS_GLOBAL_IMPLEMENT
 * \ingroup macros
 *
 * Implement needed ingredients to allow this class has only one instance. Should be
 * placed in class implementation.
 */
#define E_CLASS_GLOBAL_IMPLEMENT(klass) \
	klass* klass::global(void) {        \
		static klass obj;               \
		return &obj;                    \
	}

/**
 * \def E_CLASS_GLOBAL_EXPLICIT_DECLARE
 * \ingroup macros
 *
 * Much the same as <em>E_CLASS_GLOBAL_DECLARE</em>, except object is allocated on heap. Unlike it,
 * this macro will provide <em>init()</em> and <em>shutdown()</em> members so class instance can
 * be explicitly created and destroyed. If <em>shutdown()</em> wasn't called at the end of the program,
 * allocated memory will not be freed.
 *
 * If <em>init()</em> wasn't called before <em>global()</em>, assertion will pop up.
 */
#define E_CLASS_GLOBAL_EXPLICIT_DECLARE(klass) \
	static void init(void);                    \
	static void shutdown(void);                \
	static bool inited(void);                  \
	static klass* global(void);

/**
 * \def E_CLASS_GLOBAL_EXPLICIT_IMPLEMENT
 * \ingroup macros
 *
 * Implement needed ingredients for <em>E_CLASS_GLOBAL_EXPLICIT_DECLARE</em>. Goes where class code is implemented.
 */
#define E_CLASS_GLOBAL_EXPLICIT_IMPLEMENT(klass)                           \
	klass* klass##_instance = NULL;                                        \
                                                                           \
	void klass::init(void) {                                               \
		if(!klass##_instance)                                              \
			klass##_instance = new klass();                                \
	}                                                                      \
                                                                           \
	void klass::shutdown(void) {                                           \
		delete klass##_instance;                                           \
		klass##_instance = NULL;                                           \
	}                                                                      \
                                                                           \
	bool klass::inited(void) {                                             \
		return (klass##_instance != NULL);                                 \
	}                                                                      \
                                                                           \
	klass* klass::global(void) {                                           \
		E_ASSERT(klass##_instance != NULL && "Did you run init() first?"); \
		return klass##_instance;                                           \
	}
                                                

#ifdef __GNUC__
# define EDELIB_DEPRECATED __attribute__ ((deprecated))
#else
# define EDELIB_DEPRECATED
#endif

#ifdef HAVE_EDELIB_BASE_CONFIG_H
# include "_conf.h"
#endif

#endif
