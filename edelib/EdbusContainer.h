/*
 * $Id$
 *
 * D-BUS stuff
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

#ifndef __EDBUSCONTAINER_H__
#define __EDBUSCONTAINER_H__

#include "Debug.h"
#include "List.h"

EDELIB_NS_BEGIN

#ifndef SKIP_DOCS
template <typename T>
struct EdbusContainerImpl {
	list<T> lst;
	unsigned int ref;
};
#endif

/**
 * \ingroup dbus
 * \class EdbusContainer
 * \brief Abstract container for D-Bus containers
 *
 * EdbusContainer is a class for easier creating EdbusDict and EdbusList containers.
 * It uses implicit sharing so all concrete implementations copy internal data only
 * when is needed.
 *
 * This class should be used as base class so inherited classes can provide additional
 * funcionality.
 *
 * Inherited classes also must call unhook() member when one of their members is going
 * to write in internal EdbusContainer container.
 */
template <typename T>
class EdbusContainer {
public:
	/**
	 * Iterator type for container
	 */
	typedef typename list<T>::iterator iterator;

	/**
	 * Const iterator type for container
	 */
	typedef typename list<T>::const_iterator const_iterator;

#ifndef SKIP_DOCS
	typedef EdbusContainerImpl<T> EdbusContainerPrivate;
#endif

protected:
	/**
	 * Allows access to the private data by inherited classes
	 */
	EdbusContainerPrivate* impl;

	/**
	 * Clears internal data
	 */
	void dispose(void) {
		if(!impl)
			return;

		delete impl;
		impl = 0;
	}

	/**
	 * Do actual copying. Referece counter is set to 1.
	 * This function must be called when inherited implementations
	 * do write or change internal data
	 */
	void unhook(void) {
		E_ASSERT(impl != NULL);

		if(impl->ref == 1)
			return;

		EdbusContainerPrivate* new_one = new EdbusContainerPrivate;
		new_one->ref = 1;

		/* 
		 * Copy the content
		 *
		 * edelib::list does not have implemented copy operator
		 * and that is the way I like
		 */
		if(impl->lst.size() > 0) {
			iterator it = impl->lst.begin(), it_end = impl->lst.end();

			while(it != it_end) {
				new_one->lst.push_back(*it);
				++it;
			}
		}

		impl->ref--;
		impl = new_one;
	}

	/**
	 * Create empty container
	 */
	EdbusContainer() : impl(0) {
		impl = new EdbusContainerPrivate;
		impl->ref = 1;
	};

	/**
	 * Do a shallow copying from other container
	 */
	EdbusContainer(const EdbusContainer& other) {
		if(this == &other)
			return;

		impl = other.impl;
		other.impl->ref++;
	}

	/**
	 * Decrease reference counter and if reached 0 it will
	 * clear allocated data
	 */
	~EdbusContainer() {
		impl->ref--;

		if(impl->ref == 0)
			dispose();
	}

	/**
	 * Do a shallow copying from other container
	 */
	EdbusContainer& operator=(const EdbusContainer& other) {
		other.impl->ref++;
		impl->ref--;

		if(impl->ref == 0)
			dispose();

		impl = other.impl;
		return *this;
	}
};

EDELIB_NS_END

#endif
