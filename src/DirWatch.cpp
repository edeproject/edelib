/*
 * $Id$
 *
 * Directory changes notifier
 * Part of edelib.
 * Copyright (c) 2005-2007 EDE Authors.
 *
 * This program is licenced under terms of the 
 * GNU General Public Licence version 2 or newer.
 * See COPYING for details.
 */

#include <edelib/DirWatch.h>
#include <edelib/String.h>
#include <edelib/Vector.h>
#include <edelib/Debug.h>

#include <edelib/Directory.h>

#ifndef _GNU_SOURCE
	#define _GNU_SOURCE     // needed to get the defines for fcntl
#endif

#include <fcntl.h>

#ifdef __FreeBSD__
	#include <sys/types.h>
	#include <sys/stat.h>
#else
	#include <signal.h>
	#include <unistd.h>
#endif

#include <stdio.h>          // sscanf
#include <sys/utsname.h>    // uname

#include <time.h>   // time_t

#define ENOTIFY_SIGNAL (SIGRTMIN + 8)

EDELIB_NAMESPACE {

// content of watched directory (files, sub-directories, ...)
struct DirContentEntry {
	String   name;
	time_t   atime;
	time_t   ctime;
	time_t   mtime;
};

// watched directory
struct DirWatchEntry {
	int    fd;       // directory descriptor
	int    flags;    // user flags (DW_CREATE,...)
	String name;     // directory full path

	
	vector<DirContentEntry> content; // directory content with recorded atime/ctime/mtime
	String what_changed;             // storage for what changed
	int    changed_flag;             // DW_REPORT_CREATE, DW_REPORT_MODIFY,...
};

struct DirWatchImpl {
	DirWatchCallback* callback;
	void* callback_data;

	struct sigaction  act;
	vector<DirWatchEntry> entries;
};

DirWatch* DirWatch::pinstance = NULL;

#ifdef HAVE_DNOTIFY
void dnotify_handler(int sig, siginfo_t* si, void* data) {
	DirWatch::instance()->run_callback(si->si_fd);
}
#endif

DirWatch::DirWatch() : impl(NULL), dir_notifier(DW_NONE) {
#ifdef HAVE_DNOTIFY
	// check kernel version
	struct utsname uts;
	int major, minor, patch;
	if(uname(&uts) >= 0 && sscanf(uts.release, "%d.%d.%d", &major, &minor, &patch) == 3) {
		// >= 2.4.19
		if(major * 1000000 + minor * 1000 + patch >= 2004019 )
			dir_notifier = DW_DNOTIFY;
	} else {
		EWARNING(ESTRLOC ": dnotify disabled\n");
	}
#endif

}

DirWatch::~DirWatch() {
	if(!impl)
		return;

	unsigned int sz = impl->entries.size();

	for(unsigned int i = 0; i < sz; i++) {
		if(impl->entries[i].fd != -1) {
			close(impl->entries[i].fd);
			EDEBUG(ESTRLOC ": Closed %s\n", impl->entries[i].name.c_str());
		}
	}

	delete impl;
}

void DirWatch::init_once(void) {
	if(impl)
		return;

	impl = new DirWatchImpl;
	impl->callback = NULL;
	impl->callback_data = NULL;

#ifdef HAVE_DNOTIFY
	if(dir_notifier == DW_DNOTIFY) {
		impl->act.sa_sigaction = dnotify_handler;
		sigemptyset(&impl->act.sa_mask);
		impl->act.sa_flags = SA_SIGINFO;
		sigaction(ENOTIFY_SIGNAL, &impl->act, NULL);
	}
#endif
}

void DirWatch::init(void) {
	if(DirWatch::pinstance != NULL)
		return;
	DirWatch::pinstance = new DirWatch;
}

void DirWatch::shutdown(void) {
	if(DirWatch::pinstance == NULL)
		return;
	delete DirWatch::pinstance;
	DirWatch::pinstance = NULL;
}

DirWatch* DirWatch::instance() {
	EASSERT(DirWatch::pinstance != NULL && "Did you run DirWatch::init() ?");
	return DirWatch::pinstance;
}

bool DirWatch::add(const char* dir, int flags) {
	return DirWatch::instance()->add_entry(dir, flags);
}

bool DirWatch::remove(const char* dir) {
	return DirWatch::instance()->remove_entry(dir);
}

void DirWatch::callback(DirWatchCallback& cb, void* data) {
	DirWatch::instance()->add_callback(cb, data);
}

DirWatchNotifier DirWatch::notifier(void) {
	return DirWatch::instance()->get_notifier();
}

bool DirWatch::add_entry(const char* dir, int flags) {
	EASSERT(dir != NULL);

	init_once();
	EASSERT(impl != NULL);

	// return true even if entry is added
	// TODO: entry can exists but have fd == -1, it will be report as added; fix this
	if(have_entry(dir)) {
		EDEBUG(ESTRLOC ": already %s registered, ignoring\n", dir);
		return true;
	}

#ifdef HAVE_DNOTIFY
	if(dir_notifier == DW_DNOTIFY) {
		int fd = open(dir, O_RDONLY);

		if(fd != -1) {
			int mask = 0;

			if(flags & DW_CREATE) mask |= DN_CREATE;
			if(flags & DW_MODIFY) mask |= DN_MODIFY;
			if(flags & DW_ACCESS) mask |= DN_ACCESS;
			if(flags & DW_RENAME) mask |= DN_RENAME;
			if(flags & DW_ATTRIB) mask |= DN_ATTRIB;
			if(flags & DW_DELETE) mask |= DN_DELETE;

			if(mask) {
				if(fcntl(fd, F_SETSIG, ENOTIFY_SIGNAL) == -1) {
					close(fd);
					return false;
				}

				// a case when dnotify is disabled in kernel
				if(fcntl(fd, F_NOTIFY, mask | DN_MULTISHOT) == -1) {
					close(fd);
					return false;
				}

				DirWatchEntry e;
				e.fd = fd;
				e.name = dir;
				e.flags = flags;

				// record needed data
				fill_content(&e);

				impl->entries.push_back(e);

				return true;
			}
		}
	}
#endif

	return false;
}

bool DirWatch::have_entry(const char* dir) {
	if(!impl)
		return false;

	EASSERT(dir != NULL);

	unsigned int sz = impl->entries.size();
	DirWatchEntry* en;

	for(unsigned int i = 0; i < sz; i++) {
		en = &impl->entries[i];
		if(en->name == dir)
			return true;
	}

	return false;
}

bool DirWatch::remove_entry(const char* dir) {
	// nothing registered/added
	if(!impl)
		return false;

	EASSERT(dir != NULL);

	unsigned int sz = impl->entries.size();
	DirWatchEntry* en;

	for(unsigned int i = 0; i < sz; i++) {
		en = &impl->entries[i];

		if(en->name == dir && en->fd != -1) {
			close(en->fd);
			// mark it as closed
			en->fd = -1;
			return true;
		}
	}

	return false;
}

void DirWatch::add_callback(DirWatchCallback* cb, void* data) {
	if(!cb)
		return;

	// callback can be registered before add()
	init_once();
	EASSERT(impl != NULL);

	impl->callback = cb;
	impl->callback_data = data;
}

void DirWatch::run_callback(int fd) {
	EASSERT(impl != NULL);

	DirWatchEntry* en = scan(fd);
	if(!en || en->fd == -1)
		return;

	if(impl->callback) {
		const char* what = figure_changed(en);
		if(what) {
			// assure valid pointer
			EASSERT(en->what_changed == what && "Internal DirWatch error");
		}

		// filled from figure_changed()
		int flag = en->changed_flag;

		DirWatchCallback* cb = impl->callback;
		cb(en->name.c_str(), what, flag, impl->callback_data);
	}
}

DirWatchEntry* DirWatch::scan(int fd) {
	EASSERT(impl != NULL);

	unsigned int sz = impl->entries.size();

	for(unsigned int i = 0; i < sz; i++) {
		if(impl->entries[i].fd == fd)
			return &impl->entries[i];
	}

	return NULL;
}

void DirWatch::fill_content(DirWatchEntry* e) {
	// TODO: maybe this via plain scandir ???
	vector<String> lst;
	// list with full path
	if(!dir_list(e->name.c_str(), lst, true))
		return;

	unsigned int sz = lst.size();
	e->content.reserve(sz);

	struct stat s;
	for(unsigned int i = 0; i < sz; i++) {
		if(lstat(lst[i].c_str(), &s) != 0) {
			EWARNING(ESTRLOC ": lstat failed on %s\n", lst[i].c_str());
			continue;
		}
		
		DirContentEntry dc;
		dc.name = lst[i];
		dc.atime = s.st_atime;
		dc.mtime = s.st_mtime;
		dc.ctime = s.st_ctime;

		e->content.push_back(dc);
	}
}

DirContentEntry* in_content_list(vector<DirContentEntry>& cnt, const char* name) {
	if(!name)
		return NULL;

	unsigned int sz = cnt.size();
	for(unsigned int i = 0; i < sz; i++) {
		if(cnt[i].name == name)
			return &cnt[i];
	}

	return NULL;
}

bool in_fresh_list(vector<String>& cnt, const char* name) {
	if(!name)
		return false;

	unsigned int sz = cnt.size();
	for(unsigned int i = 0; i < sz; i++) {
		if(cnt[i] == name)
			return true;
	}

	return false;
}

/*
 * It will try to figure out what was chaged (obvious). I'm not sure
 * would this cause some races, but my tests didn't show those.
 * If was able to figure out, it will fill DirWatchEntry::what_changed string,
 * _and return pointer to it which will be passed to user callback_. If
 * something goes wrong, it will return NULL.
 *
 * Huh, sounds messy, but swappind directory contents could invalidate pointer;
 * also with this, name of changed file/dir/else will always be at safe place.
 */
const char* DirWatch::figure_changed(DirWatchEntry* e) {
	// TODO: same as above
	vector<String> fresh;
	/*
	 * Scan now 'fresh' content, comparing to stored
	 * one. First item not matching (new item/atime/mtime/ctime) will
	 * be returned. If none found NULL is returned.
	 *
	 * Here is catch too: if directory is too large scanning can take some
	 * time; in the mean time, another event could be occured in the same directory
	 * yielding miss. Really don't know how to handle these cases (at least for now).
	 */
	if(!dir_list(e->name.c_str(), fresh, true))
		return NULL;

	struct stat new_stat;
	unsigned int fresh_sz = fresh.size();
	unsigned int sz = e->content.size();
	bool rollower = false;
	bool can_add = false;

	if(fresh_sz > sz) {
		// file created
		//EDEBUG(ESTRLOC ": ==> file created\n");
		rollower = true;

		if((fresh_sz - sz) != 1)
			EWARNING(ESTRLOC ": new/stored missmatch sizes (%i) !!!\n", (fresh_sz - sz));

		for(unsigned int i = 0; i < fresh_sz; i++) {
			if(!in_content_list(e->content, fresh[i].c_str())) {
				//EDEBUG(ESTRLOC ": ==> %i created %s\n", fresh_sz-sz, fresh[i].c_str());
				e->what_changed = fresh[i].c_str();
				e->changed_flag = DW_REPORT_CREATE;
				break;
			}
		}
	} else if (fresh_sz < sz) {
		// file deleted
		//EDEBUG(ESTRLOC ": ==> file deleted\n");
		rollower = true;

		if((sz - fresh_sz) != 1)
			EWARNING(ESTRLOC ": stored/new missmatch sizes (%i) !!!\n", (sz - fresh_sz));

		for(unsigned int i = 0; i < sz; i++) {
			if(!in_fresh_list(fresh, e->content[i].name.c_str())) {
				//EDEBUG(ESTRLOC ": ==> %i deleted %s\n", sz-fresh_sz, e->content[i].name.c_str());
				e->what_changed = e->content[i].name.c_str();
				e->changed_flag = DW_REPORT_DELETE;
				break;
			}
		}
	} else {
		// nothing added/deleted then flags/size/etc. is changed
		EASSERT(fresh_sz == sz && "DirWatch internal error; sizes mismatch");

		DirContentEntry* centry = NULL;
		for(unsigned int i = 0; i < fresh_sz; i++) {
			can_add = false;

			// try to stat file first
			if(lstat(fresh[i].c_str(), &new_stat) != 0) {
				EWARNING(ESTRLOC ": lstat on %s failed\n", fresh[i].c_str());
				continue;
			}

			centry = in_content_list(e->content, fresh[i].c_str());
			if(!centry) {
				// This can happen too. I'm not sure why or kernel repor event little bit late ???
				rollower = true;
				//EDEBUG(ESTRLOC ": didnt find %s\n", fresh[i].c_str());
				break;
			} else {
			// TODO: add flags
				if(new_stat.st_atime != centry->atime) {
					//EDEBUG(ESTRLOC ": ==> modified (atime) %s\n", centry->name.c_str());
					centry->atime = new_stat.st_atime;
					can_add = true;
				}

				if(new_stat.st_ctime != centry->ctime) {
					//EDEBUG(ESTRLOC ": ==> modified (ctime) %s\n", centry->name.c_str());
					centry->ctime = new_stat.st_ctime;
					can_add = true;
				}

				if(new_stat.st_mtime != centry->mtime) {
					//EDEBUG(ESTRLOC ": ==> modified (mtime) %s\n", centry->name.c_str());
					centry->mtime = new_stat.st_mtime;
					can_add = true;
				}

				// found it, then quit from loop
				if(can_add) {
					e->what_changed = fresh[i].c_str();
					e->changed_flag = DW_REPORT_MODIFY;

					const char* ret = e->what_changed.c_str();
					return ret;
				}
			}
		}
	}

#if 0
	EDEBUG("------------ old ------------\n");
	for(int i = 0; i < e->content.size(); i++)
		EDEBUG(" %s\n", e->content[i].name.c_str());
	EDEBUG("------------ old end ------------\n");

	EDEBUG("------------ new ------------\n");
	for(int i = 0; i < fresh.size(); i++)
		EDEBUG(" %s\n", fresh[i].c_str());
	EDEBUG("------------ new end ------------\n");
#endif
	if(rollower) {
		// replace content
		//EDEBUG(ESTRLOC ": ++> rollower\n");
		e->content.clear();

		for(unsigned int i = 0; i < fresh_sz; i++) {
			if(lstat(fresh[i].c_str(), &new_stat) != 0) {
				EWARNING(ESTRLOC ": lstat failed on %s in rollower\n", fresh[i].c_str());
				continue;
			}
			
			DirContentEntry dc;
			dc.name = fresh[i];
			dc.atime = new_stat.st_atime;
			dc.mtime = new_stat.st_mtime;
			dc.ctime = new_stat.st_ctime;

			e->content.push_back(dc);
		}

		const char* ret = e->what_changed.c_str();
		return ret;
	}

	return NULL;
}

}
