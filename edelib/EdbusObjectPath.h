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

#ifndef __EDELIB_EDBUSOBJECTPATH_H__
#define __EDELIB_EDBUSOBJECTPATH_H__

#include "String.h"

EDELIB_NS_BEGIN

/**
 * \ingroup dbus
 * \class EdbusObjectPath
 * \brief Represents D-Bus object path
 *
 * This data type is necessary to correctly represents D-Bus object path
 * since D-Bus protocol limits character range that should be in object
 * path name. Also, D-Bus have different signature for object paths than
 * for ordinary strings.
 *
 * Valid path is that string that contains elements with <em>[A-Z][a-z][0-9]_</em>
 * characters, separated by <em>//</em> character. The path must begin with <em>//</em>,
 * and must not ends with it (except path contains only root). E.g.
 * \code
 *  Valid paths:
 *   /    
 *   /org/foo/baz
 *   
 *  Invalid paths:
 *   //
 *   /org/foo/baz/
 *   /org//foo/baz
 *   ...
 * \endcode
 * 
 * This class will always construct valid path, so if parameter with invalid
 * character range is given, it will ignore it.
 *
 * \todo Make this class implicitly shared
 */
class EDELIB_API EdbusObjectPath {
private:
	String val;
	EdbusObjectPath& operator=(const EdbusObjectPath&);

public:
	/**
	 * Construct root-ed path(<em>//</em>)
	 */
	EdbusObjectPath();

	/**
	 * Use str as path. If str is not valid path, EdbusObjectPath will ignore
	 * it, constructing only root-ed path
	 *
	 * \param str is string that should be valid path
	 */
	EdbusObjectPath(const char* str);

	/**
	 * Copy path from other EdbusObjectPath object
	 */
	EdbusObjectPath(const EdbusObjectPath&);

	/**
	 * Clears data
	 */
	~EdbusObjectPath();

	/**
	 * Appends str to the path. str should be path element (allowed characters
	 * without <em>//</em> character. If str is not valid element (you can validate
	 * it with valid_element() function, this function will ignore it).
	 *
	 * \todo this should probably trigger assert if element is not valid
	 * \param str is path element
	 */
	void append(const char* str);

	/**
	 * Clears data and construct root-ed path.
	 */
	void clear(void);

	/**
	 * Returns object path
	 */
	const char* path(void) const { return val.c_str(); }

	/**
	 * Validate if two paths are equal
	 *
	 * \return true if does
	 */
	bool operator==(const EdbusObjectPath& other) { return val == other.val; }

	/**
	 * Validate if two paths are not equal
	 *
	 * \return true if they are not
	 */
	bool operator!=(const EdbusObjectPath& other) { return !operator==(other); }

	/**
	 * Check if element is valid and can be used in path construction
	 *
	 * \return true if is valid
	 * \param str is path element
	 */
	static bool valid_element(const char* str);

	/**
	 * Check if str is valid path
	 *
	 * \return true if is valid
	 * \param str is full object path
	 */
	static bool valid_path(const char* str);

	/**
	 * Check if EdbusObjectPath object contains valid path. This function should
	 * always return true on fully constructed object.
	 *
	 * \return true if is valid
	 * \param path is EdbusObjectPath object
	 */
	static bool valid_path(const EdbusObjectPath& path) { return valid_path(path.path()); }
};

/**
 * \ingroup dbus
 * A simple shorcut for EdbusObjectPath append() memeber
 */
inline EdbusObjectPath& operator<<(EdbusObjectPath& p, const char* el) {
	p.append(el);
	return p;
}

EDELIB_NS_END
#endif
