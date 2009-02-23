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

#include <sys/types.h> // fork
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>    // fork, open, close, dup
#include <stdlib.h>    // getenv
#include <fcntl.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>

#include <edelib/Run.h>
#include <edelib/Missing.h>
#include <edelib/Debug.h>

/*
 * Make sure this is declared outside namespace; when shared
 * linking is done linker will see it as edelib::environ not plain environ
 */
extern char** environ;

EDELIB_NS_BEGIN

static int run_fork(const char* cmd, bool wait) {
	E_RETURN_VAL_IF_FAIL(cmd, RUN_EMPTY);

	int nulldev = -1;
	int status = 0;
	int status_ret = 0;
	
	//pid_t pid = vfork();
	pid_t pid = fork();

	if(pid == -1)
		return RUN_FORK_FAILED;

	char* shell = getenv("SHELL");
	if(!shell)
		shell = "/bin/sh";

	// run the child
	if(pid == 0) {
		char* argv[4];
		argv[0] = shell;
		argv[1] = (char*)"-c";
		argv[2] = (char*)cmd;
		argv[3] = NULL;

		/*
		 * The following is to avoid X locking when executing 
		 * terminal based application that requires user input
		 */
		if((nulldev = open("/dev/null", O_RDWR)) == -1)
			return RUN_FORK_FAILED;

		close(0); dup(nulldev);
		close(1); dup(nulldev);
		close(2); dup(nulldev);

		if(execve(argv[0], argv, environ) == -1) {
			close(nulldev);
			// should not get here
			return RUN_EXECVE_FAILED;
		}
	}

	if(wait) {
		if(waitpid(pid, &status, 0) == -1) 
			status_ret = RUN_WAITPID_FAILED;
		else {
			if(!WIFEXITED(status)) {
				status_ret = WEXITSTATUS(status);
				E_DEBUG(E_STRLOC ": run_fork(): Child '%s' died with %i\n", cmd, status_ret);
			} else if(WIFSIGNALED(status)) {
				status_ret = WTERMSIG(status);
				E_DEBUG(E_STRLOC ": run_fork(): Child '%s' signaled with %i\n", cmd,  status_ret);
			} else {
				// convert status signal to errno format
				int s = WEXITSTATUS(status);
				if(s == 127)
					status_ret = RUN_NOT_FOUND;
				else if(s == 126)
					status_ret = RUN_NOT_EXEC;
				else
					status_ret = s;
			}
		}
	}

	return status_ret;
}

int run_program(const char* cmd, bool wait) {
	return run_fork(cmd, wait);
}

int run_program_fmt(bool wait, const char* fmt, ...) {
	char buff[256];
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(buff, sizeof(buff), fmt, ap);
	va_end(ap);

	return run_program(buff, wait);
}

EDELIB_NS_END
