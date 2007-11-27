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

EDELIB_NS_BEGIN

struct DirWatchImpl { };

DirWatch::DirWatch() : impl(NULL), backend_notifier(DW_NONE) { 
}

DirWatch::~DirWatch() {
}

bool DirWatch::init_backend(void) {
	return true;
}

bool DirWatch::add_entry(const char* dir, int flags) {
	return true;
}

bool DirWatch::remove_entry(const char* dir) {
	return true;
}

void DirWatch::add_callback(DirWatchCallback* cb, void* data) {
}

EDELIB_NS_END
