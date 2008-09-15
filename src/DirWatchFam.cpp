/*
 * $Id$
 *
 * Directory changes notifier, generic (dummy version) 
 * Part of edelib.
 * Copyright (c) 2005-2007 EDE Authors.
 *
 * This program is licenced under terms of the 
 * GNU General Public Licence version 2 or newer.
 * See COPYING for details.
 */

#include <fam.h>
#include <FL/Fl.H>

EDELIB_NS_BEGIN

struct DirWatchEntry { 
	String name;         // directory full path
	int flags;           // flags for this directory
	FAMRequest request;  // request for this directory
};

struct DirWatchImpl {
	DirWatchCallback* callback;
	void* callback_data;
	FAMConnection fc;

	list<DirWatchEntry*> entries;
};

typedef list<DirWatchEntry*>::iterator DirEntryIter;

void watch_callback(int fd, void* data) {
	DirWatchImpl* impl = (DirWatchImpl*)data;
	DirWatchCallback* cb = impl->callback;
	FAMEvent ev;

	/*
	 * No callback; just clear FAM event queue or it will
	 * notify us infinitely until we do so
	 */
	if(!cb) {
		while(FAMPending(&impl->fc))
			FAMNextEvent(&impl->fc, &ev);
		return;
	}

	DirEntryIter it, it_end;
	DirWatchEntry* entry = NULL;

	while(FAMPending(&impl->fc)) {
		FAMNextEvent(&impl->fc, &ev);

		/* 
		 * FAM emit FAMExists/FAMEndExist when FAMMonitorDirectory() is called.
		 * FAMAcknowledge is emitted when whatched directory is removed from watching
		 */ 
		if(ev.code == FAMExists || ev.code == FAMEndExist || ev.code == FAMAcknowledge)
			continue;

		it = impl->entries.begin();
		it_end = impl->entries.end();

		/* 
		 * find registered directory by event request code 
		 * FIXME: this should be locked
		 */
		for(entry = NULL; it != it_end; ++it) {
			if(FAMREQUEST_GETREQNUM(&(ev.fr)) == FAMREQUEST_GETREQNUM(&((*it)->request))) {
				entry = (*it);
				break;
			}
		}

		EASSERT(entry != NULL && "Internal DirWatch (FAM) error");

		/* 
		 * process event codes but in case it was found, we will not quit
		 * the loop and return from this function since we must process all pending FAM events
		 */
		switch(ev.code) {
			case FAMCreated:
				if(entry->flags & DW_CREATE)
					cb(entry->name.c_str(), ev.filename, DW_REPORT_CREATE, impl->callback_data);
				break;
			case FAMDeleted:
				if(entry->flags & DW_DELETE)
					cb(entry->name.c_str(), ev.filename, DW_REPORT_DELETE, impl->callback_data);
				break;
			case FAMChanged:
				if(entry->flags & DW_MODIFY)
					cb(entry->name.c_str(), ev.filename, DW_REPORT_MODIFY, impl->callback_data);
				break;
			default:
				break;
		}
	}
}

DirWatch::DirWatch() : impl(NULL), backend_notifier(DW_FAM) { 
}

DirWatch::~DirWatch() {
	if(!impl)
		return;

	const FAMRequest* req;

	DirEntryIter it = impl->entries.begin(), it_end = impl->entries.end();
	for(; it != it_end; ++it) {
		req = &(*it)->request;
		FAMCancelMonitor(&impl->fc, req);
		delete *it;
	}

	impl->entries.clear();
	FAMClose(&impl->fc);
	delete impl;
}

bool DirWatch::init_backend(void) {
	impl = new DirWatchImpl;
	impl->callback = NULL;
	impl->callback_data = NULL;

	if(FAMOpen(&(impl->fc)) != 0) {
		/* can't start/connect to FAM daemon */
		delete impl;
		return false;
	}

	/* 
	 * Gamin extension which will skip FamExists event when new directory
	 * was added
	 */
#ifdef HAVE_FAMNOEXISTS
	FAMNoExists(&(impl->fc));
#endif

	/* 
	 * Use FLTK descriptor watch code. In some time it would be nice
	 * to replace it with local code so DirWatch code does not depends on FLTK
	 */
	Fl::add_fd(FAMCONNECTION_GETFD(&(impl->fc)), FL_READ, watch_callback, impl);
	return true;
}

bool DirWatch::add_entry(const char* dir, int flags) {
	EASSERT(dir != NULL);
	EASSERT(impl != NULL);

	/* check if we registered directory before */
	DirEntryIter it = impl->entries.begin(), it_end = impl->entries.end();
	for(; it != it_end; ++it) {
		if((*it)->name == dir)
			return true;
	}

	/*
	 * First check if FAM can monitor directory, and at the success
	 * register directory name
	 */
	FAMRequest fr;
	if(FAMMonitorDirectory(&(impl->fc), dir, &fr, 0) < 0)
		return false;

	DirWatchEntry* new_entry = new DirWatchEntry;
	new_entry->name = dir;
	new_entry->flags = flags;
	new_entry->request = fr;

	/* FIXME: this should be locked */
	impl->entries.push_back(new_entry);
	return true;
}

bool DirWatch::remove_entry(const char* dir) {
	EASSERT(dir != NULL);
	EASSERT(impl != NULL);

	if(impl->entries.empty())
		return false;

	const FAMRequest* req;

	DirEntryIter it = impl->entries.begin(), it_end = impl->entries.end();
	for(; it != it_end; ++it) {
		if((*it)->name == dir) {
			/* TODO: return value checks */
			req = &(*it)->request;
			FAMCancelMonitor(&(impl->fc), req);

			/* FIXME: this should be locked */
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
