/*
 * $Id$
 *
 * D-Bus stuff
 * Part of edelib.
 * Copyright (c) 2008 EDE Authors.
 *
 * This program is licenced under terms of the 
 * GNU General Public Licence version 2 or newer.
 * See COPYING for details.
 */

#include <stdio.h>
#include <edelib/EdbusList.h>
#include <edelib/Debug.h>

EDELIB_NS_BEGIN

EdbusList::EdbusList(bool a) : array_mode(a) { }

void EdbusList::append(const EdbusData& val) {
	if(array_mode && impl->lst.size() > 0) {
		const_iterator it = begin();

		if((*it).type() != val.type()) {
			E_WARNING("Array contains different types. Ignoring...\n");
			return;
		}
	}

	unhook();
	impl->lst.push_back(val);
}

void EdbusList::clear(void) {
	unhook();
	impl->lst.clear();
}

void EdbusList::remove(const EdbusData& val) {
	unhook();

	EdbusList::iterator it = impl->lst.begin(), it_end = impl->lst.end();

	for(; it != it_end; ++it) {
		if((*it) == val) {
			impl->lst.erase(it);
			return;
		}
	}
}

void EdbusList::remove_all(const EdbusData& val) {
	unhook();

	EdbusList::iterator it = impl->lst.begin(), it_end = impl->lst.end();

	while(it != it_end) {
		if((*it) == val)
			it = impl->lst.erase(it);
		else
			++it;
	}
}

bool EdbusList::operator==(const EdbusList& other) const {
	/* 
	 * if we are array but not other, there is no
	 * point to check stuff
	 */
	if(array_mode != other.array_mode)
		return false;

	return impl->lst == other.impl->lst;
}

EdbusDataType EdbusList::value_type(void) {
	E_ASSERT(size() > 0 && "Can't get key type on empty container");

	if(array_mode) {
		EdbusList::const_iterator it = begin();
		return (*it).type();
	}

	return EDBUS_TYPE_INVALID;
}

bool EdbusList::value_type_is_container(void) {
	E_ASSERT(size() > 0 && "Can't get value type on empty container");

	if(array_mode) {
		EdbusList::const_iterator it = begin();
		if(EdbusData::basic_type(*it))
			return false;
		if((*it).type() == EDBUS_TYPE_VARIANT)
			return false;
		return true;
	}

	/* then is struct */
	return false;
}

EdbusList::const_iterator EdbusList::begin(void) const {
	return impl->lst.begin();
}

EdbusList::const_iterator EdbusList::end(void) const {
	return impl->lst.end();
}

unsigned int EdbusList::size(void) const {
	return impl->lst.size();
}

EDELIB_NS_END
