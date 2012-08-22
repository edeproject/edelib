/*
 * $Id: EdbusConnection.h 3367 2012-08-10 13:01:03Z karijes $
 *
 * D-BUS stuff
 * Copyright (c) 2008-2012 edelib authors
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

#ifndef __EDELIB_EDBUSINTROSPECTION_H__
#define __EDELIB_EDBUSINTROSPECTION_H__

#include "String.h"

EDELIB_NS_BEGIN

#ifndef SKIP_DOCS
struct EdbusIntrospImpl;
#endif

/**
 * \ingroup dbus
 * \class EdbusIntrospection
 * \brief D-Bus introspection facility
 *
 * This class handles constructing valid introspection string from given object paths and their introspection XML parts.
 * It will make sure valid XML string is served for set of objects, no matter how their hierarchy looks. It will also fill
 * the gap of missing nodes to reach object path.
 *
 * DBus introspection is best described on http://dbus.freedesktop.org/doc/dbus-specification.html#introspection-format.
 */
class EdbusIntrospection {
private:
	EdbusIntrospImpl *impl;
	void dispose(void);
public:
	/**
	 * Create introspection object. Introspection data still does not exists.
	 */
	EdbusIntrospection();

	/**
	 * Destroys object.
	 */
	~EdbusIntrospection();

	/**
	 * Create introspection object from another object.
	 */
	EdbusIntrospection(const EdbusIntrospection& other);

	/**
	 * Assign another object.
	 */
	EdbusIntrospection& operator=(const EdbusIntrospection& other);

	/**
	 * Append XML chunk describing object with object path. XML chunk must start with &lt;node&gt; tag
	 * and should not have freedesktop DTD header, since it will be appended when xml_string() was called.
	 *
	 * \return true if data with object is appended or false if not, probably due bad object path or NULL-ed string
	 * \param is valid object path
	 * \param xmlstr is XML chunk describing this object path
	 */
	bool append(const char* path, const char* xmlstr);

	/**
	 * Get raw XML for this object; DTD header is not appended. If you want to read or serve XML introspection
	 * for desired object, use xml_string().
	 *
	 * \return associated cstring (no need to free it) with this object path. If not found, returns NULL.
	 * \param path is valid object path
	 */
	const char* data(const char* path);
};

EDELIB_NS_END
#endif
 
