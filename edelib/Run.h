/*
 * $Id$
 *
 * Run external program
 * Part of edelib.
 * Copyright (c) 2005-2007 EDE Authors.
 *
 * This program is licenced under terms of the 
 * GNU General Public Licence version 2 or newer.
 * See COPYING for details.
 */

#ifndef __RUN_H__
#define __RUN_H__

#include "econfig.h"

EDELIB_NS_BEGIN

enum {
	RUN_NOT_FOUND       = 65535,  ///< executable not found
	RUN_EMPTY           = 65534,  ///< given parameter is NULL
	RUN_NOT_EXEC        = 65533,  ///< given parameter is not executable on system 
	RUN_FORK_FAILED	    = 65532,  ///< internal fork failed
	RUN_WAITPID_FAILED  = 65531,  ///< internal waitpid failed
	RUN_EXECVE_FAILED   = 65530,  ///< internal execve failed
	RUN_PTY_FAILED      = 65529,  ///< todo
	RUN_USER_CANCELED   = 65528   ///< todo
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
 * \param root todo
 */
EDELIB_API int run_program(const char* cmd, bool wait = true, bool root = false);

EDELIB_NS_END
#endif // __RUN_H__
