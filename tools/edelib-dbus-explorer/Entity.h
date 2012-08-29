/*
 * $Id: String.h 2839 2009-09-28 11:36:20Z karijes $
 *
 * Copyright (c) 2005-2012 edelib authors
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

#ifndef __EDELIB_DBUS_EXPLORER_ENTITY_H__
#define __EDELIB_DBUS_EXPLORER_ENTITY_H__

#include <edelib/List.h>

enum EntityType {
	ENTITY_NONE,
	ENTITY_PROPERTY,
	ENTITY_SIGNAL,
	ENTITY_METHOD
};

enum ArgDirection {
	DIRECTION_IN,
	DIRECTION_OUT,
	DIRECTION_NONE
};

/* contains dbus signature string and variable name */
struct ArgSignature {
	char *sig;
	char *name;
	char *access;    /* only for properties */
	ArgDirection direction;

	ArgSignature();
	~ArgSignature();
};

typedef EDELIB_NS_PREPEND(list<ArgSignature*>) ArgSignatureList;
typedef EDELIB_NS_PREPEND(list<ArgSignature*>::iterator) ArgSignatureListIt;

/*
 * Entity is interface meta member; this means, entity can be member, signal or
 * property. It also have details about the member, like return type and number of
 * parameters; with these informations, we will know how to call it and with what types
 */
class Entity {
private:
	EntityType tp;
	char       *name;
	char       *path;
	char       *interface;
	char       *service;
	char       *doc;
	ArgSignatureList args;
public:
	Entity() : tp(ENTITY_NONE), name(NULL), path(NULL), interface(NULL), service(NULL), doc(NULL) { }
	~Entity();

	void       set_type(EntityType t) { tp = t; }
	EntityType get_type(void) { return tp; }

	void  set_name(const char *n);
	char *get_name(void) { return name; }

	void  set_path(const char *p);
	char *get_path(void) { return path; }

	void  set_interface(const char *i);
	char *get_interface(void) { return interface; }

	void  set_service(const char *s);
	char *get_service(void) { return service; }

	void  set_doc(const char *s);
	char *get_doc(void) { return doc; }

	void append_arg(const char *name, const char *type, ArgDirection direction, const char *access = NULL);

	/* get prototype as readable string */
	bool get_prototype(char *buf, int bufsz);

	/* get prototype as scheme code */
	bool get_prototype_as_scheme(char *buf, int bufsz);
};

#endif
