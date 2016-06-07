/*
 * $Id$
 *
 * Run external program
 * Copyright (c) 2005-2013 edelib authors
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

#ifndef __EDELIB_RUN_H__
#define __EDELIB_RUN_H__

#include "edelib-global.h"

EDELIB_NS_BEGIN

enum {
	RUN_NOT_FOUND      = 65535, ///< executable not found
	RUN_EMPTY          = 65534, ///< given parameter is NULL
	RUN_NOT_EXEC       = 65533, ///< given parameter is not executable on system 
	RUN_FORK_FAILED	   = 65532, ///< internal fork failed
	RUN_WAITPID_FAILED = 65531, ///< internal waitpid failed
	RUN_EXECVE_FAILED  = 65530, ///< internal execve failed
	RUN_PIPE_FAILED    = 65529, ///< internal pipe failed
	RUN_NO_ACCESS      = 65528  ///< not enough permissions to execute it
};

/**
 * Executes the given program until it completes. If full path to the executable was given, it will be directly called; if not,
 * PATH environment variable should contain a path where executable is placed.
 *
 * Please note how some programs run without parameters (but internaly are executed via shell) can set <em>errno</em> to 2 which
 * is usually interpreted as ENOENT (or program does not exists); for examle <em>tar</em> is known for this.
 *
 * \return 0 if starting and quitting program went fine; otherwise return one of RUN_* codes or errno value for not checked codes
 * \param fmt is printf-like formatted string
 */
EDELIB_API int run_sync(const char *fmt, ...);

/**
 * Same as run_sync(), except it will run command without blocking.
 */
EDELIB_API int run_async(const char *fmt, ...);

/**
 * Run program without blocking, but return successfully started child PID value.
 *
 * \return the same values as run_sync()
 * \param child_pid if not NULL will be set PID value of started child
 * \param fmt is printf-like formatted string
 */
EDELIB_API int run_async_with_pid(int *child_pid, const char *fmt, ...);

EDELIB_NS_END
#endif
