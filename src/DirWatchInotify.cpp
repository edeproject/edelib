/*
 * $Id$
 *
 * Directory changes notifier based on inotify
 * Part of edelib.
 * Copyright (c) 2005-2007 EDE Authors.
 *
 * This program is licenced under terms of the 
 * GNU General Public Licence version 2 or newer.
 * See COPYING for details.
 */

#include <sys/inotify.h>
#include <unistd.h>
#include <errno.h>
#include <FL/Fl.h>

#define EVENT_SIZE (sizeof (struct inotify_event))
#define BUFF_LEN (1024 * (EVENT_SIZE + 16))

EDELIB_NS_BEGIN

struct DirWatchEntry {
	String name;      // directory full path
	int flags;        // flags for this directory
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

void watch_callback(int fd, void* data) { 
	char buff[BUFF_LEN];
	int len = 0;
	int i = 0;
	int myflags = 0;
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
				if(event->mask & IN_DELETE)
					myflags = DW_REPORT_DELETE;
				else if(event->mask & IN_CREATE)
					myflags = DW_REPORT_CREATE;
				else if(event->mask & IN_MODIFY)
					myflags = DW_REPORT_MODIFY;
				else if(event->mask & IN_ATTRIB)
					myflags = DW_REPORT_MODIFY;
				else {
					/* unknown flag; should not happened; just continue */
					continue;
				}

				impl->tmp_storage = event->name;
				EDEBUG(ESTRLOC ": Changed %s in %s\n", event->name, (*it)->name.c_str());

				/* call callback and quit loop */
				cb((*it)->name.c_str(), impl->tmp_storage.c_str(), myflags, impl->callback_data);
				return;
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
		/* TODO: remove watch */
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
	EASSERT(dir != NULL);
	EASSERT(impl != NULL);

	/* 
	 * if entry already exists, just skip it since calling inotify code on the already
	 * watched will reset flags
	 */
	DirEntryIter it = impl->entries.begin(), it_end = impl->entries.end();
	for(; it != it_end; ++it) {
		if((*it)->name == dir)
			return false;
	}

	int inotify_flags = 0;
	if(flags & DW_CREATE) inotify_flags |= IN_CREATE;
	if(flags & DW_DELETE) inotify_flags |= IN_DELETE;
	if(flags & DW_MODIFY) inotify_flags |= IN_MODIFY;
	if(flags & DW_ATTRIB) inotify_flags |= IN_ATTRIB;
	/* ignore other flags */

	int fd = inotify_add_watch(impl->inotify_fd, dir, inotify_flags);
	if(fd < 0)
		return false;

	DirWatchEntry* new_entry = new DirWatchEntry;
	new_entry->name = dir;
	new_entry->flags = flags;
	new_entry->dir_fd = fd;

	impl->entries.push_back(new_entry);
	return true;
}

bool DirWatch::remove_entry(const char* dir) {
	EASSERT(dir != NULL);
	EASSERT(impl != NULL);

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
	EASSERT(impl != NULL);

	impl->callback = cb;
	impl->callback_data = data;
}

EDELIB_NS_END
