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
