/*
 * $Id: List.h 2839 2009-09-28 11:36:20Z karijes $
 *
 * Functional approach for lists
 * Copyright (c) 2005-2011 edelib authors
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

#ifndef __EDELIB_FUNCTIONAL_H__
#define __EDELIB_FUNCTIONAL_H__

#include "Debug.h"

EDELIB_NS_BEGIN

/**
 * \defgroup functional Functional handling for edelib::List and similar constructs
 */

/**
 * \ingroup functional
 * Filter given container by calling <i>func</i> on each element. If <i>func</i>
 * returns true for given element, it will be copied to other list.
 */
template <typename T, typename F>
unsigned int filter(const F& func, const T& container, T& ret) {
	typename T::const_iterator it  = container.begin();
	typename T::const_iterator ite = container.end();

	for(; it != ite; ++it) {
		if(func(*it))
			ret.push_back(*it);
	}

	return ret.size();
}

/**
 * \ingroup functional
 * Apply <i>func</i> on each element of given container and add it to <i>ret</i> container.
 */
template <typename T, typename F>
void map(F& func, const T& container, T& ret) {
	typename T::const_iterator it  = container.begin();
	typename T::const_iterator ite = container.end();

	for(; it != ite; ++it)
		ret.push_back(func(*it));
}

/**
 * \ingroup functional
 * Reduce all elements from container using <i>func</i> as function. The best example of this
 * is to sum all elements, like:
 *  \code
 *    int sum(int a, int b) {
 *       return a + b;
 *    }
 *
 *    int ret;
 *    list <int> lst; // [1,2,3,4,5]
 *    reduce(sum, lst, ret);
 *    // result will be in form 1 + 2 + 3 + 4 + 5
 *  \endcode
 */
template <typename T, typename R, typename F>
void reduce(F& func, const T& container, R& ret) {
	unsigned int sz = container.size();
	if(sz == 0) {
		// nothing
	} else if(sz == 1)
		ret = *container.begin();
	else {
		typename T::const_iterator it  = container.begin();
		typename T::const_iterator it2 = it;
		++it2;
		typename T::const_iterator ite = container.end();

		ret = func(*it, *it2);
		for(++it2; it2 != ite; ++it2)
			ret = func(*it2, ret);
	}
}

/**
 * \ingroup functional
 * Traverse container calling <i>func</i> on each element. Returns nothing.
 */
template <typename T, typename F>
void for_each(const F& func, const T& container) {
	typename T::const_iterator it  = container.begin();
	typename T::const_iterator ite = container.end();

	for(; it != ite; ++it)
		func(*it);
}

/**
 * \ingroup functional
 * Same as above <i>for_each</i>, but with additional <i>void*</i> parameter
 * that is given to function as second parameter (function is called as <i>func(val, void*)</i>
 */
template <typename T, typename F>
void for_each(const F& func, const T& container, void* p) {
	typename T::const_iterator it  = container.begin();
	typename T::const_iterator ite = container.end();

	for(; it != ite; ++it)
		func(*it, p);
}

EDELIB_NS_END
#endif
