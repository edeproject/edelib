/*
 * $Id$
 *
 * This class enables to "chat" with terminal programs synchronously
 * Copyright (c) 2006-2009 edelib authors
 *
 * This file was a part of the KDE project, module kdesu.
 * Copyright (C) 1999,2000 Geert Jansen <jansen@kde.org>
 *
 * This file contains code from TEShell.C of the KDE konsole.
 * Copyright (c) 1997,1998 by Lars Doelle <lars.doelle@on-line.de>
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/ioctl.h>
#include <sys/select.h>

#if defined(__SVR4) && defined(sun)
#include <stropts.h>
#include <sys/stream.h>
#endif

#include <edelib/Nls.h>
#include <edelib/Debug.h>
#include <edelib/PtyProcess.h>
#include <edelib/Pty.h>

EDELIB_NS_BEGIN

static int strpos(const char *str, char c) {
	unsigned int len = strlen(str);

	for(unsigned int i = 0; i < len; i++)
		if(str[i] == c) 
			return i;

	return -1;
}

int PtyProcess::wait_ms(int fd,int ms) {
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 1000*ms;

	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(fd,&fds);
	return select(fd+1, &fds, 0L, 0L, &tv);
}

bool PtyProcess::check_pid(pid_t pid) {
	return (kill(pid, 0) == 0);
}

int PtyProcess::check_pid_exited(pid_t pid) {
	int state, ret;

	ret = waitpid(pid, &state, WNOHANG);
	if(ret < 0)
		return Error;

	if(ret == pid) {
		if (WIFEXITED(state))
			return WEXITSTATUS(state);
		if (WIFSIGNALED(state) && WTERMSIG(state)==SIGSEGV)
			return Crashed;
		return Killed;
	}

	return NotExited;
}

class PtyProcess::PtyProcessPrivate {
public:
	char **env;

	~PtyProcessPrivate() {
		if(!env)
			return;
		for(int i = 0; env[i]; i++)
			free(env[i]);
	}
};

PtyProcess::PtyProcess() {
	m_bTerminal = false;
	m_bErase = false;
	m_pPTY = 0L;
	d = new PtyProcessPrivate;
	d->env = 0;
	m_Pid = 0;
	m_Inbuf = m_TTY = m_Exit = m_Command = 0;
}

int PtyProcess::init() {
	delete m_pPTY;

	m_pPTY = new PTY();
	m_Fd = m_pPTY->getpt();
	if(m_Fd < 0)
		return -1;

	if((m_pPTY->grantpt() < 0) || (m_pPTY->unlockpt() < 0)) {
		E_WARNING(E_STRLOC ": master setup failed.\n");
		m_Fd = -1;
		return -1;
	}

	if(m_TTY) 
		free(m_TTY);

	m_TTY = strdup(m_pPTY->ptsname());

	if(m_Inbuf) 
		free(m_Inbuf);

	m_Inbuf = 0;
	return 0;
}


PtyProcess::~PtyProcess() {
	E_DEBUG(E_STRLOC " : killing pid %d\n",m_Pid);
	if(m_Pid) 
		kill(m_Pid, SIGSTOP); // Terminate child process - Vedran

	if(m_TTY) free(m_TTY);
	if(m_Inbuf) free(m_Inbuf);

	delete m_pPTY;
	delete d;
}

void PtyProcess::set_environment(const char **env) {
	int i;

	// deallocate old environment store
	if(d->env) {
		for(i = 0; env[i]; i++)
			free(d->env[i++]);
	}
	
	// count how many items are in env
	int n = 0;
	while(env[n])
		n++;

	d->env = (char**)malloc((n + 2) * sizeof(char *));
	
	// copy env to d->env
	for(i = 0; env[i]; i++)
		d->env[i] = strdup(env[i]); 

	d->env[i] = NULL;
}

char **PtyProcess::environment() const {
	return d->env;
}

/*
 * Read one line of input. The terminal is in canonical mode, so you always
 * read a line at at time
 */
char *PtyProcess::read_line(bool block) {
	int pos;
	char *ret = 0;

	if(m_Inbuf && strlen(m_Inbuf)>0) {
		pos = strpos(m_Inbuf,'\n');

		if(pos == -1) {
			ret = strdup(m_Inbuf);
			free(m_Inbuf);
			m_Inbuf = 0;
		} else {
			// ret = part of m_Inbuf before \n
			// m_Inbuf = part of m_Inbuf after \n
			ret = strdup(m_Inbuf);
			free(m_Inbuf);
			m_Inbuf = strdup(ret + pos + 1);
			ret[pos+1] = '\0';
		}

		return ret;
	}

	int flags = fcntl(m_Fd, F_GETFL);
	if(flags < 0) {
		E_WARNING(E_STRLOC ": fcntl not working - %d\n", errno);
		return ret;
	}

	if(block)
		flags &= ~O_NONBLOCK;
	else
		flags |= O_NONBLOCK;

	if(fcntl(m_Fd, F_SETFL, flags) < 0) {
	   // We get an error here when the child process has closed 
	   // the file descriptor already.
	   return ret;
	}

	int nbytes;
	char buf[256];

	while(1) {
		nbytes = read(m_Fd, buf, 255);

		if(nbytes == -1) {
			if (errno == EINTR && block)
				continue;
			break; 
		}

		if(nbytes == 0)
			break;

		buf[nbytes] = '\000';

		if (m_Inbuf) 
			m_Inbuf = (char*)realloc(m_Inbuf, strlen(m_Inbuf) + nbytes + 1);
		else {
			m_Inbuf = (char*)malloc(nbytes + 1);
			m_Inbuf[0] = 0;
		}

		strcat(m_Inbuf, buf);
		ret = strdup(m_Inbuf);

		// only one line...
		pos = strpos(ret,'\n');
		if (pos != -1) { 
			free (m_Inbuf);
			m_Inbuf = strdup(ret + pos + 1);
			ret[pos+1] = '\0';
		}

		break;
	}

	return ret;
}

void PtyProcess::write_line(const char *line, bool addnl) {
	if(!line)
		return;

	int len = strlen(line);

	if(len > 0)
		write(m_Fd, line, len);

	if(addnl)
		write(m_Fd, "\n", 1);
}

void PtyProcess::unread_line(const char *line, bool addnl) {
	char *tmp = (char*) malloc(strlen(line) + 1);
	strcpy(tmp,line);
	if(addnl)
		strcat(tmp, "\n");

	if (m_Inbuf) {
		char *tmp2 = (char*)malloc(strlen(m_Inbuf) + strlen(tmp) + 1);
		strcpy(tmp2,tmp);
		strcat(tmp2,m_Inbuf);

		free(m_Inbuf);
		m_Inbuf = tmp2;
		free(tmp);
	} else 
		m_Inbuf = tmp;
}

/*
 * Fork and execute the command. This returns in the parent.
 */
int PtyProcess::exec(const char *command, const char **args) {
	E_DEBUG(E_STRLOC ": running '%s'\n", command);
	if(init() < 0)
		return -1;

	// Open the pty slave before forking. See SetupTTY()
	E_DEBUG(E_STRLOC ": pty: %s\n", m_TTY);
	int slave = open(m_TTY, O_RDWR);

	if(slave < 0) {
		E_WARNING(E_STRLOC ": could not open slave pty.\n");
		return -1;
	}

	errno = 0;
	if((m_Pid = fork()) == -1) {
		E_WARNING(E_STRLOC ": fork failed with '%s'\n", strerror(errno));
		return -1;
	}

	// Parent
	if(m_Pid) {
		close(slave);
		return 0;
	}

	// Child
	if(setup_tty(slave) < 0)
		_exit(1);

	if (d->env) {
		for(int i = 0; d->env[i]; i++)
			putenv(d->env[i]);
	}

	execv(command, (char**)args);
	_exit(1);

	return -1; /* never reached */
}

/*
 * Wait until the terminal is set into no echo mode. At least one su
 * (RH6 w/ Linux-PAM patches) sets noecho mode AFTER writing the Password:
 * prompt, using TCSAFLUSH. This flushes the terminal I/O queues, possibly
 * taking the password	with it. So we wait until no echo mode is set
 * before writing the password.
 * Note that this is done on the slave fd. While Linux allows tcgetattr() on
 * the master side, Solaris doesn't.
 */
int PtyProcess::wait_slave() {
	int slave = open(m_TTY, O_RDWR);
	if (slave < 0) {
		E_WARNING(E_STRLOC ": could not open slave tty\n");
		return -1;
	}

//	  kdDebug(900) << k_lineinfo << "Child pid " << m_Pid << endl;

	struct termios tio;
	errno = 0;

	while (1) {
		if (!check_pid(m_Pid)) {
			close(slave);
			return -1;
		}

		if(tcgetattr(slave, &tio) < 0) {
			E_WARNING(E_STRLOC ": tcgetattr() failed with '%s'\n", strerror(errno));
			close(slave);
			return -1;
		}

		if(tio.c_lflag & ECHO) {
			E_DEBUG(E_STRLOC ": echo mode still on\n");
			wait_ms(slave,100);
			continue;
		}

		break;
	}

	close(slave);
	return 0;
}

int PtyProcess::enable_local_echo(bool enable) {
	int slave = open(m_TTY, O_RDWR);
	if (slave < 0) {
		E_WARNING(E_STRLOC ": could not open slave tty\n");
		return -1;
	}

	struct termios tio;
	errno = 0;

	if(tcgetattr(slave, &tio) < 0) {
		E_WARNING(E_STRLOC ": tcgetattr() failed with '%s'\n", strerror(errno));
		close(slave); 
		return -1;
	}

	if(enable)
		tio.c_lflag |= ECHO;
	else
		tio.c_lflag &= ~ECHO;

	if(tcsetattr(slave, TCSANOW, &tio) < 0) {
		E_WARNING(E_STRLOC ": tcsetattr() failed with '%s'\n", strerror(errno));
		close(slave); 
		return -1;
	}

	close(slave);
	return 0;
}

/*
 * Copy output to stdout until the child process exists, or a line of output
 * matches `m_Exit'.
 * We have to use waitpid() to test for exit. Merely waiting for EOF on the
 * pty does not work, because the target process may have children still
 * attached to the terminal.
 */
int PtyProcess::wait_for_child(void) {
	int retval = 1;

	fd_set fds;
	FD_ZERO(&fds);
	errno = 0;
	
	while(1) {
		FD_SET(m_Fd, &fds);
		int ret = select(m_Fd+1, &fds, 0L, 0L, 0L);
		if (ret == -1) {
			if (errno != EINTR) {
				E_WARNING(E_STRLOC ": select() failed with '%s'\n", strerror(errno));
				return -1;
			}

			ret = 0;
		}
		
		if(ret) {
			int len;

			for(char* line = read_line(false); line && strlen(line) > 0; line = read_line(false)) {
				if(m_Exit) {
					len = strlen(m_Exit);

					if(len > 0 && !strncasecmp(line, m_Exit, len))
						kill(m_Pid, SIGTERM);
				}

				if(m_bTerminal) {
					fputs(line, stdout);
					fputc('\n', stdout);
				}
			}
		}

		ret = check_pid_exited(m_Pid);
		if(ret == Error) {
			if (errno == ECHILD) 
				retval = 0;
			else 
				retval = 1;
			break;
		} else if(ret == Killed || ret == Crashed) {
			retval = 0;
			break;
		} else if (ret == NotExited) {
		// keep checking
		} else {
			retval = ret;
			break;
		}
	}

	return retval;
}

/*
 * Creates a new session. The filedescriptor "fde" should be
 * connected to the tty. It is closed after the tty is reopened to make it
 * our controlling terminal. This way the tty is always opened at least once
 * so we'll never get EIO when reading from it.
 */
int PtyProcess::setup_tty(int fde)
{
	// Reset signal handlers
	for(int sig = 1; sig < NSIG; sig++)
		signal(sig, SIG_DFL);
	signal(SIGHUP, SIG_IGN);

	// Close all file handles
	struct rlimit rlp;
	getrlimit(RLIMIT_NOFILE, &rlp);
	for(int i = 0; i < (int)rlp.rlim_cur; i++) {
		if (i != fde) 
			close(i);
	}

	// Create a new session.
	setsid();

	// Open slave. This will make it our controlling terminal
	errno = 0;
	int slave = open(m_TTY, O_RDWR);
	if (slave < 0) {
		E_WARNING(E_STRLOC ": could not opent slave side ('%s')\n", strerror(errno));
		return -1;
	}
	close(fde);

#if defined(__SVR4) && defined(sun)
	// Solaris STREAMS environment.
	// Push these modules to make the stream look like a terminal.
	ioctl(slave, I_PUSH, "ptem");
	ioctl(slave, I_PUSH, "ldterm");
#endif

#ifdef TIOCSCTTY
	ioctl(slave, TIOCSCTTY, NULL);
#endif

	// Connect stdin, stdout and stderr
	dup2(slave, 0); 
	dup2(slave, 1); 
	dup2(slave, 2);

	if(slave > 2)
		close(slave);

	// Disable OPOST processing. Otherwise, '\n' are (on Linux at least)
	// translated to '\r\n'.
	struct termios tio;
	errno = 0;

	if(tcgetattr(0, &tio) < 0) {
		E_WARNING(E_STRLOC ": tcgetattr() failed with '%s'\n", strerror(errno));
		return -1;
	}
	tio.c_oflag &= ~OPOST;

	if(tcsetattr(0, TCSANOW, &tio) < 0) {
		E_WARNING(E_STRLOC ": tcsetattr() failed with '%s'\n", strerror(errno));
		return -1;
	}

	return 0;
}

EDELIB_NS_END
