/*
 * $Id$
 *
 * Run external program
 * Copyright (c) 2005-2009 edelib authors
 * Part of the code was based on GLib's gspawn.c, copyright (c) 2000 Red Hat
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

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <sys/param.h> /* BSD's keeps BSD specific macro (for #ifdef) in this file */

#ifdef BSD
# include <limits.h>      /* PATH_MAX */
# include <sys/signal.h>  /* SIGPIPE, SIG_DFL */
#endif

#if defined(sun) || defined(__sun)
# include <limits.h>      /* PATH_MAX */
# include <signal.h>      /* signal() */
#endif

#include <edelib/Run.h>
#include <edelib/Missing.h>
#include <edelib/Debug.h>

#define CMD_BUF_SIZE 128

EDELIB_NS_BEGIN

/*
 * A bunch of these functions are shamelessly stolen from glib (gspawn.c) and/or are
 * served as ideas for writing the current ones. Also, the way of using pipes with grandchild process
 * is also from glib. What to say, a good library :)
 */

static char** cmd_split(const char* cmd, int* count) {
	int   sz = 10;
	int   i = 0;
	char* c = strdup(cmd);

	char** arr = (char**)malloc(sizeof(char*) * sz);
	if(!arr)
		return NULL;

	for(char* p = strtok(c, " "); p; p = strtok(NULL, " ")) {
		if(i >= sz) {
			sz *= 2;
			arr = (char**)realloc(arr, sizeof(char*) * sz);
		}
		arr[i++] = strdup(p);
	}

	arr[i] = NULL;
	free(c);
	*count = i;

	return arr;
}

static int convert_from_errno(int e, int fallback) {
	switch(e) {
		case ENOEXEC:
			return RUN_NOT_EXEC;
		case ENOENT:
			return RUN_NOT_FOUND;
		case EACCES:
			return RUN_NO_ACCESS;
	}

	return fallback;
}

static void close_and_invalidate(int* fd) {
	/* 
	 * not needed E_RETURN_IF_FAIL() or will receive 'Condition failed' each
	 * time when program wasn't found
	 */
	if(*fd == -1)
		return;

	close(*fd);
	*fd = -1;
}

static void write_int(int fd, int val) {
	E_RETURN_IF_FAIL(fd != -1);

	char* buf = (char*)&val;
	int bufsz = sizeof(val), count = 0;

	while(bufsz > 0) {
		count = write(fd, buf, bufsz);
		if(count < 0) {
			if(errno != EINTR)
				return;
		} else {
			bufsz -= count;
			buf += count;
		}
	}
}

static bool read_ints(int fd, int* buf, int bufsz, int* int_read) {
	E_RETURN_VAL_IF_FAIL(fd != -1, false);

	unsigned int bytes, part;
	errno = 0;
	bytes = part = *int_read = 0;

	while(1) {
		/* extra safety */
		if(bytes >= sizeof(int) * 2)
			break;

		while(1) {
			part = read(fd, (char*)buf + bytes, sizeof(int) * bufsz - bytes);
			if(errno != EINTR)
				break;
		}

		if(part > 0) {
			bytes += part;
		} else if(part == 0) {
			break;
		} else {
			/* bad happened; bail out */
			return false;
		}
	}

	*int_read = (int)(bytes / sizeof(int));
	return true;
}

static void exec_cmd_via_shell(char* program, char** args, int count) {
	char** new_args = (char**)malloc(sizeof(char*) * count + 2);

	new_args[0] = (char*)"/bin/sh";
	new_args[1] = program;

	int i, j;
	for(i = 1, j = 2; args[i]; i++, j++)
		new_args[j] = args[i];

	new_args[j] = NULL;

	execv(new_args[0], (char* const*)new_args); 
	free(new_args);
}

static void exec_cmd(const char* cmd, int child_err_report_fd) {
	/* let it be closed on exec */
	if(child_err_report_fd != -1)
		fcntl(child_err_report_fd, F_SETFD, FD_CLOEXEC); 

	int errnosv, count = 0;
	char** args = cmd_split(cmd, &count);

	if(!args) {
		errnosv = ENOMEM;

		if(child_err_report_fd != -1) {
			/* report it to the pipe if it was used*/
			write_int(child_err_report_fd, RUN_EXECVE_FAILED);
			write_int(child_err_report_fd, errnosv);
			_exit(1);
		} else {
			/* or directly if wasn't */
			_exit(errnosv);
		}
	}

	/* see if it has a path component in itself */
	if(strchr(args[0], '/') != NULL) {
		execv(args[0], args);

		/* execute it then via shell if failed */
		if(errno == ENOEXEC)
			exec_cmd_via_shell(args[0], args, count);
	} else {
		/*
		 * search PATH then; here will not be used file_path() or similar checker
		 * but execve() will be directly called on each path component and if failed,
		 * it will go to the next available
		 */
		char name[PATH_MAX], *path, *path_copy;
		
		path = getenv("PATH");
		if(!path) {
			/* in glib was stated that '.' is put last for security so I'm using that here too */
			path = (char*)"/bin:/usr/bin:.";
		}

		path_copy = strdup(path);

		if(path_copy) {
			for(char* t = strtok(path_copy, ":"); t; t = strtok(NULL, ":")) {
				snprintf(name, sizeof(name), "%s/%s", t, args[0]);

				/* execute it */
				execv(name, args);

				/* or try with shell instead */
				if(errno == ENOEXEC)
					exec_cmd_via_shell(name, args, count);

				/* these indicates that current entry wasn't found; go to the next */
				if(errno == EACCES || errno == ENOENT || errno == ENOTDIR)
					continue;

				/* if we got here, it means it is bad; break it and let errno be reported via pipe */
				errnosv = errno;
				break;
			}

			free(path_copy);
		}
	}

	/* quickly save it before any further stdlib calls */
	errnosv = errno;

	/* got here; everything failed so report it then */
	for(int i = 0; args[i]; i++)
		free(args[i]);
	free(args);

	if(child_err_report_fd != -1) {
		/* report for async code */
		write_int(child_err_report_fd, RUN_EXECVE_FAILED);
		write_int(child_err_report_fd, errnosv);
		_exit(1);
	} else {
		/* report for sync code */
		_exit(errnosv);
	}
}

static int fork_child_async(const char* cmd, int* child_pid) {
	pid_t pid, grandchild_pid;
	int child_pid_report_pipe[2] = {-1, -1};
	int child_err_report_pipe[2] = {-1, -1};
	int null_dev;

	/* by default is assumed how command wasn't found */
	int ret = RUN_NOT_FOUND;

	errno = 0;
	if(pipe(child_pid_report_pipe) != 0) {
		E_WARNING(E_STRLOC ": pipe() failed with '%s'\n", strerror(errno));
		return RUN_PIPE_FAILED;
	}

	if(pipe(child_err_report_pipe) != 0) {
		E_WARNING(E_STRLOC ": pipe() failed with '%s'\n", strerror(errno));
		return RUN_PIPE_FAILED;
	}

	pid = fork();

	if(pid < 0) {
		E_WARNING(E_STRLOC ": fork() failed with '%s'\n", strerror(errno));
		return RUN_FORK_FAILED;
	} else if(pid == 0) {
		signal(SIGPIPE, SIG_DFL);

		close_and_invalidate(&child_pid_report_pipe[0]);
		close_and_invalidate(&child_err_report_pipe[0]);

		/* TODO stdin, stdout, stderr */

		grandchild_pid = fork();

		if(grandchild_pid < 0) {
			/* report -1 as returned value */
			write_int(child_pid_report_pipe[1], grandchild_pid);
			write_int(child_err_report_pipe[1], RUN_FORK_FAILED);
			_exit(1);
		} else if(grandchild_pid == 0) {
			/* 
			 * second child code is here; from now on every error
			 * is reported via pipe
			 */
			null_dev = open("/dev/null", O_RDWR);

			if(null_dev == -1) {
				write_int(child_pid_report_pipe[1], grandchild_pid);
				write_int(child_err_report_pipe[1], errno);
				_exit(1);
			}

			/* close on exec */
			fcntl(child_err_report_pipe[1], F_SETFD, FD_CLOEXEC);

			/* just send stdin, stdout, stderr to null dev */
			close(0); 
			dup(null_dev);

			close(1); 
			dup(null_dev);

			close(2); 
			dup(null_dev);

			exec_cmd(cmd, child_err_report_pipe[1]);
		} else {
			/* child (a grandchild parent) is here; report it's pid before normal exit */
			write_int(child_pid_report_pipe[1], grandchild_pid);
			close_and_invalidate(&child_err_report_pipe[1]);
			_exit(0);
		}
	} else {
		/* parent */
		int status, buf[2], n_ints = 0;

		close_and_invalidate(&child_pid_report_pipe[1]);
		close_and_invalidate(&child_err_report_pipe[1]);

		/* reap intermediate child */
		while(waitpid(pid, &status, 0) < 0) {
			if(errno == EINTR)
				continue;
			break;
		}

		if(!read_ints(child_err_report_pipe[0], buf, 2, &n_ints))
			goto fail;

		/* child signaled some error; inspect it and bail out */
		if(n_ints >= 2) {
			ret = convert_from_errno(buf[1], buf[0]);
			goto fail;
		}

		/* get pid from grandchild pid, this is the pid of actual forked program */
		n_ints = 0;
		if(!read_ints(child_pid_report_pipe[0], buf, 1, &n_ints)) {
			ret = RUN_PIPE_FAILED;
			goto fail;
		}

		if(n_ints < 1) {
			ret = RUN_PIPE_FAILED;
			goto fail;
		}

		if(child_pid)
			*child_pid = buf[0];

		close_and_invalidate(&child_err_report_pipe[0]);
		close_and_invalidate(&child_pid_report_pipe[0]);
		/* everything went fine */
		return 0;
	}

fail:
	/* we got some error in first child; reap it so it does not become a zombie */
	if(pid > 0) {
		while(waitpid(pid, NULL, 0) < 0) {
			if(errno != EINTR)
				break;
		}
	}

	close_and_invalidate(&child_pid_report_pipe[0]);
	close_and_invalidate(&child_pid_report_pipe[1]);
	close_and_invalidate(&child_err_report_pipe[0]);
	close_and_invalidate(&child_err_report_pipe[1]);

	return ret;
}

static int fork_child_sync(const char* cmd) {
	pid_t pid;
	int null_dev;

	pid = fork();
	if(pid == -1)
		return RUN_FORK_FAILED;
	else if(pid == 0) {
		/* child */
		null_dev = open("/dev/null", O_RDWR);
		if(null_dev == -1)
			return RUN_FORK_FAILED;

		/* just send stdin, stdout, stderr to null dev */
		close(0); 
		dup(null_dev);

		close(1); 
		dup(null_dev);

		close(2); 
		dup(null_dev);

		exec_cmd(cmd, -1);
	}

	/* parent */
	int status, ret = -1;
	if(waitpid(pid, &status, 0) == -1)
		return RUN_WAITPID_FAILED;

	if(!WIFEXITED(status))
		ret = WEXITSTATUS(status);
	else if(WIFSIGNALED(status))
		ret = WTERMSIG(status);
	else {
		int s = WEXITSTATUS(status);
		ret = convert_from_errno(s, s);
	}

	return ret;
}

int run_program(const char* cmd, bool wait) {
	if(wait)
		return fork_child_sync(cmd);
	else
		return fork_child_async(cmd, 0);
}

int run_program_fmt(bool wait, const char* fmt, ...) {
	char buf[CMD_BUF_SIZE];
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);

	if(wait)
		return fork_child_sync(buf);
	else
		return fork_child_async(buf, 0);
}

int run_sync(const char* fmt, ...) {
	E_ASSERT(fmt != NULL);

	char buf[CMD_BUF_SIZE];
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);

	return fork_child_sync(buf);
}

int run_async(const char* fmt, ...) {
	E_ASSERT(fmt != NULL);

	char buf[CMD_BUF_SIZE];
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);

	return fork_child_async(buf, 0);
}

EDELIB_NS_END
