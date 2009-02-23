/*
 * $Id$
 *
 * Directory changes notifier
 * Copyright (c) 2005-2007 edelib authors
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


#include <sys/inotify.h>
#include <unistd.h>
#include <errno.h>
#include <FL/Fl.H>

#define EVENT_SIZE (sizeof (struct inotify_event))
#define BUFF_LEN (1024 * (EVENT_SIZE + 16))

EDELIB_NS_BEGIN

struct DirWatchEntry {
	String name;      // directory full path
	int dir_fd;       // directory descriptor
};

struct DirWatchImpl {
	DirWatchCallback* callback;
	void* callback_data;
	int   inotify_fd;

	String tmp_storage;
	list<DirWatchEntry*> entries;
};

typedef list<DirWatchEntry*>::iterator DirEntryIter;

static void watch_callback(int fd, void* data) { 
	char buff[BUFF_LEN];
	int len = 0;
	int i = 0;
	int report = 0;
	struct inotify_event* event;
	DirWatchImpl* impl = (DirWatchImpl*)data;
	DirEntryIter it, it_end;
	it_end = impl->entries.end();

again:
	len = read(fd, buff, BUFF_LEN);
	if(len < 0) {
		if(errno == EINTR)
			goto again;
		else
			return;
	} else if(len == 0) {
		/* buffer too small */
		return;
	}

	DirWatchCallback* cb = impl->callback;
	if(!cb)
		return;

	while(i < len) {
		event = (struct inotify_event*)&buff[i];

		/* scan for directory descriptor */
		for(it = impl->entries.begin(); it != it_end; ++it) {
			if((*it)->dir_fd == event->wd) {
				/*
				 * When file is moved (but not deleted) in/out watched directory, inotify
				 * will not emit IN_CREATE/IN_DELETE but IN_MOVED_TO/IN_MOVED_FROM instead.
				 * To allow compatibility with Gamin backend we will see IN_MOVED_XXX events
				 * as entry creating/deleting. The same applies for watch_callback() reports.
				 */
				if(event->mask & (IN_DELETE | IN_MOVED_FROM))
					report = DW_REPORT_DELETE;
				else if(event->mask & (IN_CREATE | IN_MOVED_TO))
					report = DW_REPORT_CREATE;
				else if(event->mask & IN_MODIFY)
					report = DW_REPORT_MODIFY;
				else if(event->mask & IN_ATTRIB)
					report = DW_REPORT_MODIFY;
				else {
					/* unknown flag; should not happened; just continue */
					continue;
				}

				/* call callback but do not quit since we can get continuous events */
				cb((*it)->name.c_str(), event->name, report, impl->callback_data);
			}
		}

		i += EVENT_SIZE + event->len;
	}
}

DirWatch::DirWatch() : impl(NULL), backend_notifier(DW_INOTIFY) { 
}

DirWatch::~DirWatch() {
	if(!impl)
		return;

	DirEntryIter it = impl->entries.begin(), it_end = impl->entries.end();
	for(; it != it_end; ++it) {
		inotify_rm_watch(impl->inotify_fd, (*it)->dir_fd);
		delete *it;
	}

	impl->entries.clear();
	close(impl->inotify_fd);
	delete impl;
}

bool DirWatch::init_backend(void) {
	impl = new DirWatchImpl;
	impl->callback = NULL;
	impl->callback_data = NULL;

	impl->inotify_fd = inotify_init();
	if(impl->inotify_fd < 0)
		return false;

	/* 
	 * Use FLTK descriptor watch code. In some time it would be nice
	 * to replace it with local code so DirWatch code does not depends on FLTK
	 */
	Fl::add_fd(impl->inotify_fd, watch_callback, impl);

	return true;
}

bool DirWatch::add_entry(const char* dir, int flags) {
	E_ASSERT(dir != NULL);
	E_ASSERT(impl != NULL);

	/* 
	 * if entry already exists, just skip it since calling inotify code on the already
	 * watched will reset flags
	 */
	DirEntryIter it = impl->entries.begin(), it_end = impl->entries.end();
	for(; it != it_end; ++it) {
		if((*it)->name == dir)
			return true;
	}

	int inotify_flags = 0;
	if(flags & DW_CREATE) inotify_flags |= IN_CREATE | IN_MOVED_TO;
	if(flags & DW_DELETE) inotify_flags |= IN_DELETE | IN_MOVED_FROM;
	if(flags & DW_MODIFY) inotify_flags |= IN_MODIFY;
	if(flags & DW_ATTRIB) inotify_flags |= IN_ATTRIB;
	/* ignore other flags */

	int fd = inotify_add_watch(impl->inotify_fd, dir, inotify_flags);
	if(fd < 0)
		return false;

	DirWatchEntry* new_entry = new DirWatchEntry;
	new_entry->name = dir;
	new_entry->dir_fd = fd;

	impl->entries.push_back(new_entry);
	return true;
}

bool DirWatch::remove_entry(const char* dir) {
	E_ASSERT(dir != NULL);
	E_ASSERT(impl != NULL);

	if(impl->entries.empty())
		return false;

	DirEntryIter it = impl->entries.begin(), it_end = impl->entries.end();
	for(; it != it_end; ++it) {
		if((*it)->name == dir) {
			/* TODO: return value checks */
			inotify_rm_watch(impl->inotify_fd, (*it)->dir_fd);

			delete *it;
			impl->entries.erase(it);
			return true;
		}
	}

	return false;
}

void DirWatch::add_callback(DirWatchCallback* cb, void* data) {
	/* allow NULL callbacks */
	E_ASSERT(impl != NULL);

	impl->callback = cb;
	impl->callback_data = data;
}

EDELIB_NS_END
