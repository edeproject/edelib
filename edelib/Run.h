/*
 * $Id$
 *
 * Run external program
 * Copyright (c) 2005-2007 edelib authors
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

#ifndef __RUN_H__
#define __RUN_H__

#include "edelib-global.h"

EDELIB_NS_BEGIN

enum {
	RUN_NOT_FOUND      = 65535, ///< executable not found
	RUN_EMPTY          = 65534, ///< given parameter is NULL
	RUN_NOT_EXEC       = 65533, ///< given parameter is not executable on system 
	RUN_FORK_FAILED	   = 65532, ///< internal fork failed
	RUN_WAITPID_FAILED = 65531, ///< internal waitpid failed
	RUN_EXECVE_FAILED  = 65530  ///< internal execve failed
};

/**
 * Executes given program. Path for the program can be specified or not, but
 * for case when is not specified, environment PATH should contain a path
 * where is executable placed.
 *
 * If parameter <em>wait</em> is set to true (default), this function will wait
 * untill started program is not exited; in that case will return code from
 * started program. On other hand, will just run program (will not check if program
 * can be run or not) and return 0, which is default value for succesfull execution.
 *
 * \return 0 if starting and quitting program went fine; otherwise return one of
 * above codes, or errno value for not checked codes
 * \param cmd is commad to be executed with optional full path and parameters
 * \param wait if is true (default) function will not exit until program exists
 * \deprecated in a favor of run_sync() and run_async()
 */
EDELIB_API int run_program(const char* cmd, bool wait = true) EDELIB_DEPRECATED;

/**
 * Same as run_program(), but run printf-like constructed command
 * \deprecated in a favor of run_sync() and run_async()
 */
EDELIB_API int run_program_fmt(bool wait, const char* fmt, ...) EDELIB_DEPRECATED;

/**
 * Executes the given program. If full path to the executable was given, it will be 
 * directly called; if not, PATH environment variable should contain a path where
 * executable is placed.
 *
 * This function will run a command and wait until it finishes.
 * \return 0 if starting and quitting program went fine; otherwise return one of
 * above codes, or errno value for not checked codes
 * \param fmt is printf-like formated string
 */
EDELIB_API int run_sync(const char* fmt, ...);

/**
 * Same as run_sync(), except it will run command without blocking
 */
EDELIB_API int run_async(const char* fmt, ...);

EDELIB_NS_END
#endif // __RUN_H__
