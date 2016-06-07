/*
 * $Id$
 *
 * Descriptor monitoring functions
 * Copyright (c) Bill Spitzak and the others
 * Copyright (c) 2008 edelib authors
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

#include <sys/select.h>
#include <time.h>
#include <stdlib.h>

#include <edelib/Listener.h>

EDELIB_NS_BEGIN

struct FD {
	int fd;
	short events;
	void (*cb)(int, void*);
	void* arg;
};

static FD  *fd_array = 0;
static int fd_array_size = 0;
static int nfds = 0;
static int maxfd = 0; // largest descriptor number known

static fd_set fdsets[3];

void listener_add_fd(int fd, int when, void(*cb)(int, void*), void *arg) {
	listener_remove_fd(fd, when);

	int i = nfds++;
	if(i >= fd_array_size) {
		FD *tmp;
		fd_array_size = 2 * fd_array_size + 1;

		if(!fd_array)
			tmp = (FD*)malloc(sizeof(FD) * fd_array_size);
		else
			tmp = (FD*)realloc(fd_array, sizeof(FD) * fd_array_size);

		if(!tmp)
			return;
		fd_array = tmp;
	}

	fd_array[i].fd = fd;
	fd_array[i].events = when;
	fd_array[i].cb = cb;
	fd_array[i].arg = arg;

	if(when & LISTENER_READ)
		FD_SET(fd, &fdsets[0]);
	if(when & LISTENER_WRITE)
		FD_SET(fd, &fdsets[1]);
	if(when & LISTENER_EXCEPT)
		FD_SET(fd, &fdsets[2]);
	
	if(fd > maxfd)
		maxfd = fd;
}

void listener_remove_fd(int fd, int when) {
	int i, j;
	maxfd = -1; // recalculate maxfd in the fly

	for(i = j = 0; i < fd_array_size; i++) {
		if(fd_array[i].fd == fd) {
			int e = fd_array[i].events & ~when;

			// no events left, delete this fd
			if(!e)
				continue;
			fd_array[i].events = e;
		}

		if(fd_array[i].fd > maxfd)
			maxfd = fd_array[i].fd;
		// remove it down in the array if necessary
		if(j < i)
			fd_array[j] = fd_array[i];
		j++;
	}

	fd_array_size = j;

	if(when & LISTENER_READ)
		FD_CLR(fd, &fdsets[0]);
	if(when & LISTENER_WRITE)
		FD_CLR(fd, &fdsets[1]);
	if(when & LISTENER_EXCEPT)
		FD_CLR(fd, &fdsets[2]);
}

double listener_wait(double t) {
	fd_set fdtmp[3];
	fdtmp[0] = fdsets[0];
	fdtmp[1] = fdsets[1];
	fdtmp[2] = fdsets[2];

	int n;

	if(t < 2147483.648) {
		timeval tval;
		tval.tv_sec = (int)t;
		tval.tv_usec = (int)(1000000 * (t - tval.tv_sec));

		n = select(maxfd + 1, &fdtmp[0], &fdtmp[1], &fdtmp[2], &tval);
	} else {
		n = select(maxfd + 1, &fdtmp[0], &fdtmp[1], &fdtmp[2], 0);
	}

	if(n > 0) {
		for(int i = 0; i < fd_array_size; i++) {
			int f = fd_array[i].fd;
			short ev = 0;

			if(FD_ISSET(f, &fdtmp[0]))
				ev |= LISTENER_READ;
			if(FD_ISSET(f, &fdtmp[1]))
				ev |= LISTENER_WRITE;
			if(FD_ISSET(f, &fdtmp[2]))
				ev |= LISTENER_EXCEPT;

			if(fd_array[i].events & ev)
				fd_array[i].cb(f, fd_array[i].arg);
		}
	}

	return n;
}

EDELIB_NS_END
