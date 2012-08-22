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

#include <string.h>
#include <edelib/EdbusIntrospection.h>
#include <edelib/EdbusObjectPath.h>
#include <edelib/Debug.h>
#include <edelib/TiXml.h>
#include <edelib/String.h>
#include <edelib/List.h>
#include <edelib/Version.h>
#include <edelib/Missing.h>

#define STR_CMP(s1, s2) (strcmp((s1), (s2)) == 0)

#define DTD_HEADER														             \
"<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\"\n" \
"  \"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">\n"              \
"<!-- edelib " EDELIB_VERSION " -->\n"

#define EMPTY_NODE DTD_HEADER "<node>\n</node>\n"

EDELIB_NS_BEGIN

struct ObjectData {
	String object;
	String xml;
};

typedef list<ObjectData*> ObjectDataList;
typedef list<ObjectData*>::iterator ObjectDataListIt;

struct EdbusIntrospImpl {
	unsigned int   refs;
	ObjectDataList objects;
};

EdbusIntrospection::EdbusIntrospection() : impl(NULL) {
	impl = new EdbusIntrospImpl;
	impl->refs =  1;
}

EdbusIntrospection::~EdbusIntrospection() {
	if(!impl)
		return;

	impl->refs--;
	if(impl->refs == 0)
		dispose();
}

void EdbusIntrospection::dispose(void) {
	if(!impl)
		return;

	ObjectDataListIt it = impl->objects.begin(), ite = impl->objects.end();
	while(it != ite) {
		delete *it;
		it = impl->objects.erase(it);
	}

	delete impl;
	impl = NULL;
}

EdbusIntrospection::EdbusIntrospection(const EdbusIntrospection& other) {
	if(this == &other)
		return;

	impl = other.impl;
	other.impl->refs++;
}

EdbusIntrospection& EdbusIntrospection::operator=(const EdbusIntrospection& other) {
	other.impl->refs++;
	impl->refs--;

	if(impl->refs == 0)
		dispose();

	impl = other.impl;
	return *this;
}

bool EdbusIntrospection::append(const char *path, const char *xmlstr) {
	E_ASSERT(impl != NULL);
	E_RETURN_VAL_IF_FAIL(path != NULL, false);
	E_RETURN_VAL_IF_FAIL(xmlstr != NULL, false);

	/* first, make sure we have valid xmlstr */
	TiXmlDocument part;
	part.Parse(xmlstr);

	TiXmlElement *root = part.RootElement();
	if(!root || !STR_CMP(root->Value(), "node")) {
		E_WARNING(E_STRLOC ": Invalid XML for '%s' object. It must start with 'node' node\n", path);
		return false;
	}

	ObjectData *d = new ObjectData;
	d->object = path;
	d->xml = DTD_HEADER;
	d->xml += xmlstr;
	impl->objects.push_back(d);
	return true;
}

#define DBUS_PATH_CHAR '/'
#define DBUS_PATH_STR  "/"

static void for_each_path_item(const char *path, bool (*func)(const char *item, void *data), void *data) {
	if(path[0] == DBUS_PATH_CHAR && func(DBUS_PATH_STR, data))
		return;

	char *saveptr, *dup = strdup(path);
	for(char *i = strtok_r(dup, DBUS_PATH_STR, &saveptr); i; i = strtok_r(NULL, DBUS_PATH_STR, &saveptr)) {
		if(func(i, data))
			break;
	}

	free(dup);
}

const char *EdbusIntrospection::data(const char *path) {
	E_ASSERT(impl != NULL);
	E_RETURN_VAL_IF_FAIL(path != NULL, EMPTY_NODE);

	unsigned int len = edelib_strnlen(path, 256);
	E_RETURN_VAL_IF_FAIL(len > 0, EMPTY_NODE);

	ObjectDataListIt it = impl->objects.begin(), ite = impl->objects.end();
	//for(; it != ite; ++it) {
	//for_each_path_item(
	//}


	for(; it != ite; ++it) {
		/* if we have full match, return full xml */
		if((*it)->object == path)
			return (*it)->xml.c_str();

		/* check if we got subpath of object path; in that case, return next node in object path */
		if((len < (*it)->object.length()) &&
		   (strncmp(path, (*it)->object.c_str(), len) == 0))
		{
			String buf, &oo = (*it)->object;
			buf.reserve(32);


			/* to handle this case
			 *  /org/foo
			 *  /org/fooe
			 */
			if(oo[len] != DBUS_PATH_CHAR) return EMPTY_NODE;
			for(unsigned int i = len + 1; i < oo.length() && oo[i] != DBUS_PATH_CHAR; i++)
				buf += oo[i];

			E_DEBUG("XXXXX\n");

			if(buf.empty()) return EMPTY_NODE;

			/* register it */
			ObjectData *od = new ObjectData;
			od->object = buf;
			od->xml = DTD_HEADER;
			od->xml += "<node>\n <node name=\"";
			od->xml += buf;
			od->xml += "\"/>\n</node>\n";

			impl->objects.push_back(od);

			return od->xml.c_str();
		}
	}

	return EMPTY_NODE;
}

EDELIB_NS_END
