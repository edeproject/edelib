/*
 * $Id$
 *
 * A class for handling pseudoterminals (PTYs)
 * Copyright (c) 2006-2008 edelib authors
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

#ifndef _GNU_SOURCE
#define _GNU_SOURCE   /* Needed for getpt, ptsname in glibc 2.1.x systems */
#endif

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <termios.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ioctl.h>

#include <edelib/Pty.h>

#if defined(__osf__) || defined(__CYGWIN__) || defined(HAVE_PTY_H)
#include <pty.h>
#endif

#ifdef HAVE_LIBUTIL_H
	#include <libutil.h>
#elif defined(HAVE_UTIL_H)
	#include <util.h>
#endif

#ifndef __THROW
	#define __THROW
#endif

#ifdef HAVE_GRANTPT
extern "C" int grantpt(int fd) __THROW;
#endif

#ifdef HAVE_PTSNAME
extern "C" char * ptsname(int fd) __THROW;
#endif

#ifdef HAVE_UNLOCKPT
extern "C" int unlockpt(int fd) __THROW;
#endif

#ifdef HAVE__GETPTY
extern "C" char *_getpty(int *, int, mode_t, int);
#endif

#define MAXNAME 30

EDELIB_NS_BEGIN

#if !(defined(HAVE_GETPT) && defined(HAVE_PTSNAME))
static void *qalloc(void *ptr, size_t size) {
	if(ptr)
		return realloc(ptr, size);
	else
		return malloc(size);
}
#endif

PTY::PTY() {
	ptyfd = -1;
	ptyname = ttyname = 0;
}

PTY::~PTY() {
	if(ptyfd >= 0)
		close(ptyfd);

	if(ptyname) free(ptyname);
	if(ttyname) free(ttyname);
}

int PTY::getpt() {
#if defined(HAVE_GETPT) && defined(HAVE_PTSNAME)
	// 1: UNIX98: preferred way
	ptyfd = ::getpt();
	ttyname = strdup(::ptsname(ptyfd));
	return ptyfd;

#elif defined(HAVE_OPENPTY)
	// 2: BSD interface
	// More preferred than the linux hacks
	char name[MAXNAME];
	int master_fd, slave_fd;

	if(openpty(&master_fd, &slave_fd, name, 0L, 0L) != -1)	{
		ttyname = (char*)qalloc(ttyname, MAXNAME);
		strncpy(ttyname, name, MAXNAME);
		name[5]='p';

		ptyname = (char*)qalloc(ptyname, MAXNAME);
		strncpy(ptyname, name, MAXNAME);
		close(slave_fd);

		ptyfd = master_fd;
		return ptyfd;
	}

	ptyfd = -1;
	return -1;

#elif defined(HAVE__GETPTY)
	// 3: Irix interface
	int master_fd;
	char *tmp = _getpty(&master_fd,O_RDWR,0600,0);

	if(tmp) {
		ttyname = strdup(tmp);
		ptyfd = master_fd;
	} else
		ptyfd = -1;

	return ptyfd;

#else
	// 4: Open terminal device directly
	// 4.1: Try /dev/ptmx first. (Linux w/ Unix98 PTYs, Solaris)

	ptyfd = open("/dev/ptmx", O_RDWR);
	if(ptyfd >= 0) {
		ptyname = strdup("/dev/ptmx");

#ifdef HAVE_PTSNAME
		ttyname = strdup(::ptsname(ptyfd));
		return ptyfd;
#elif defined (TIOCGPTN)
		int ptyno;
		if(ioctl(ptyfd, TIOCGPTN, &ptyno) == 0) {
			ttyname = (char*)qalloc(ttyname,MAXNAME);
			snprintf(ttyname, MAXNAME-1, "/dev/pts/%d", ptyno);
			return ptyfd;
		}
#endif
		close(ptyfd);
	}

	// 4.2: Try /dev/pty[p-e][0-f] (Linux w/o UNIX98 PTY's)
	for (const char *c1 = "pqrstuvwxyzabcde"; *c1 != '\0'; c1++) {
		for (const char *c2 = "0123456789abcdef"; *c2 != '\0'; c2++) {
			ptyname = (char*)qalloc(ptyname,strlen("/dev/pty12"));
			ttyname = (char*)qalloc(ttyname,strlen("/dev/tty12"));

			sprintf(ptyname, "/dev/pty%c%c", *c1, *c2);
			sprintf(ttyname, "/dev/tty%c%c", *c1, *c2);

			if(access(ptyname, F_OK) < 0)
				goto linux_out;

			ptyfd = open(ptyname, O_RDWR);
			if (ptyfd >= 0)
				return ptyfd;
		}
	}

linux_out:
	for(int i=0; i < 256; i++) {
		ptyname = (char*)qalloc(ptyname,MAXNAME);
		ttyname = (char*)qalloc(ttyname,MAXNAME);

		snprintf(ptyname, MAXNAME-1, "/dev/ptyp%d", i);
		snprintf(ttyname, MAXNAME-1, "/dev/ttyp%d", i);

		if(access(ptyname, F_OK) < 0)
			break;

		ptyfd = open(ptyname, O_RDWR);
		if(ptyfd >= 0)
			return ptyfd;
	}

	// Other systems ??
	ptyfd = -1;
	return -1;

#endif // HAVE_GETPT && HAVE_PTSNAME
}


int PTY::grantpt() {
	if(ptyfd < 0)
		return -1;

#ifdef HAVE_GRANTPT
	return ::grantpt(ptyfd);
#elif defined(HAVE_OPENPTY)
	// the BSD openpty() interface chowns the devices properly for us, no need to do this at all
	return 0;
#else
	// FIXME: what???
	if(strncmp(ptyname, "/dev/pty", 8) != 0)
		return 0;

	// no pty capabilities
	return -1;
#endif // HAVE_GRANTPT
}

int PTY::unlockpt()
{
	if(ptyfd < 0)
		return -1;

#ifdef HAVE_UNLOCKPT
	// (Linux w/ glibc 2.1, Solaris, ...)
	return ::unlockpt(ptyfd);
#elif defined(TIOCSPTLCK)
	// Unlock pty (Linux w/ UNIX98 PTY's & glibc 2.0)
	int flag = 0;
	return ioctl(ptyfd, TIOCSPTLCK, &flag);
#else
	// Other systems (Linux w/o UNIX98 PTY's, ...)
	return 0;
#endif
}

const char *PTY::ptsname() {
	if (ptyfd < 0)
		return 0;

	return ttyname;
}

EDELIB_NS_END
