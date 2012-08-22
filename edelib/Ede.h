/*
 * $Id: String.h 2594 2009-03-25 14:54:54Z karijes $
 *
 * EDE specific code
 * Copyright (c) 2010-2012 edelib authors
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

#ifndef __EDELIB_EDE_H__
#define __EDELIB_EDE_H__

#include "edelib-global.h"
#include "Nls.h"

EDELIB_NS_BEGIN

/**
 * \class ApplicationBootstrap
 * \brief EDE application bootstrapper
 *
 * This class should not be called directly, it will be called from EDE_APPLICATION macro. It will essentially
 * setup needed assets required by application, like initialize default theme, load icons and such, and after application
 * ends, it will cleanup loaded data.
 *
 * When this class is used (via EDE_APPLICATION), it must be linked with libedelib_gui (and dependencies), as will load GUI specific code.
 */
class EDELIB_API ApplicationBootstrap {
private:
	int flags;
public:
	/** Initialize constructor. */
	ApplicationBootstrap(const char *name, const char *nls_path);
	/** Cleanup. */
	~ApplicationBootstrap();
};

EDELIB_NS_END

/**
 * \def EDE_APPLICATION_WITH_NLS_PATH
 * \ingroup macros
 *
 * Initialize common EDE code with application name (must be binary name) and full path to locale directory.
 */
#define EDE_APPLICATION_WITH_NLS_PATH(name, path)   \
	extern int FL_NORMAL_SIZE;                      \
	FL_NORMAL_SIZE = 12;                            \
	EDELIB_NS_PREPEND(ApplicationBootstrap) __ede_application_bootstrap(name, path)

/**
 * \def EDE_CONSOLE_APPLICATION_WITH_NLS_PATH
 * \ingroup macros
 *
 * Same as EDE_APPLICATION_WITH_NLS_PATH but only for console applications.
 */
#define EDE_CONSOLE_APPLICATION_WITH_NLS_PATH(name, path) \
	EDELIB_NS_PREPEND(nls_support_init(name, path)

/**
 * \def EDE_APPLICATION
 * \ingroup macros
 *
 * Initialize common EDE code with application name. This macro uses <em>PREFIX</em> value
 * to setup correct path for localization data, which means the header (<em>Ede.h</em>) should
 * be included after <em>PREFIX</em> definition.
 */

/**
 * \def EDE_CONSOLE_APPLICATION
 * \ingroup macros
 *
 * Same as EDE_APPLICATION but is intended for console applications.
 */
#ifdef PREFIX
# define EDE_APPLICATION(name)         EDE_APPLICATION_WITH_NLS_PATH(name, PREFIX"/share/locale")
# define EDE_CONSOLE_APPLICATION(name) EDE_CONSOLE_APPLICATION_WITH_NLS_PATH(name, PREFIX"/share/locale")
#else
# define EDE_APPLICATION(name)         EDE_APPLICATION_WITH_NLS_PATH(name, "dummy_path")
# define EDE_CONSOLE_APPLICATION(name) EDE_CONSOLE_APPLICATION_WITH_NLS_PATH(name, "dummy_path")
#endif

#endif
