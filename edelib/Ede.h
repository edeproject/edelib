/*
 * $Id: String.h 2594 2009-03-25 14:54:54Z karijes $
 *
 * EDE specific code
 * Copyright (c) 2010 edelib authors
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

/**
 * \def EDE_APPLICATION_WITH_NLS_PATH
 * \ingroup macros
 *
 * Initialize common EDE code with application name (must be binary name) and full path to locale directory.
 */
#define EDE_APPLICATION_WITH_NLS_PATH(name, path)    \
	extern int FL_NORMAL_SIZE;                       \
	FL_NORMAL_SIZE = 12;                             \
	EDELIB_NS_PREPEND(nls_support_init(name, path))

/**
 * \def EDE_APPLICATION
 * \ingroup macros
 *
 * Initialize common EDE code with application name. This macro uses <em>PREFIX</em> value
 * to setup correct path for localization data, which means the header (<em>Ede.h</em>) should
 * be included after <em>PREFIX</em> definition.
 */
#ifdef PREFIX
# define EDE_APPLICATION(name) EDE_APPLICATION_WITH_NLS_PATH(name, PREFIX"/share/locale")
#else
# define EDE_APPLICATION(name) EDE_APPLICATION_WITH_NLS_PATH(name, "dummy_path")
#endif

#endif
