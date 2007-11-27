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
#include <edelib/Debug.h>
#include <edelib/Directory.h>
#include <edelib/List.h>

/*
 * NOTE: DirWatch.cpp revision 2081 contained dnotify implementation
 * but was removed due bad performances (that is why gamin was invented).
 * This note is for me in case I need to dig out dnotify support again :-P
 */
EDELIB_NS_BEGIN

DirWatch* DirWatch::pinstance = NULL;

bool DirWatch::init(void) {
	if(DirWatch::pinstance == NULL)
		DirWatch::pinstance = new DirWatch;

	return DirWatch::pinstance->init_backend();
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

EDELIB_NS_END

#ifdef HAVE_INOTIFY
	#include "DirWatchInotify.cpp"
#elif HAVE_FAM
	#include "DirWatchFam.cpp"
#else
	#include "DirWatchGeneric.cpp"
#endif
