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

#include <stdlib.h>
#include <edelib/Missing.h>
#include <edelib/Debug.h>
#include <edelib/String.h>
#include "Entity.h"

EDELIB_NS_USING(list)
EDELIB_NS_USING(String)

#define MAX_STRSZ 256
#define STR_CMP(s1, s2) (strcmp((s1), (s2)) == 0)

/* try to return DBus signature tags as readable content, like 'a' -> Array or 's' -> String */
static void signature_to_readable(const char *sig, String &ret) {
	int len = edelib_strnlen(sig, MAX_STRSZ);
	if(len == 1) {
		switch(sig[0]) {
			case 'y':
				ret = "byte";
				return;
			case 'b':
				ret = "bool";
				return;
			case 'n':
				ret = "int16";
				return;
			case 'q':
				ret = "uint16";
				return;
			case 'i':
				ret = "int32";
				return;
			case 'u':
				ret = "uint32";
				return;
			case 'x':
				ret = "int64";
				return;
		   	case 't':
				ret = "uint64";
				return;
			case 'd':
				ret = "double";
				return;
			case 's':
				ret = "string";
				return;
			case 'o':
				ret = "object_path";
				return;
			case 'g':
				ret = "signature";
				return;
			case 'a':
				ret = "array";
				return;
			case 'v':
				ret = "variant";
				return;
		}
	}

	ret = sig;
}

ArgSignature::ArgSignature() : sig(NULL), name(NULL), access(NULL), direction(DIRECTION_NONE) { }

ArgSignature::~ArgSignature() {
	if(sig) free(sig);
	if(name) free(name);
	if(access) free(access);
}

Entity::Entity() : tp(ENTITY_NONE), name(NULL), path(NULL), interface(NULL) { }

Entity::~Entity() {
	if(name) free(name);
	if(path) free(path);
	if(interface) free(interface);

	ArgSignatureListIt it = args.begin(), ite = args.end();
	for(; it != ite; it = args.erase(it))
		delete *it;
}

void Entity::set_name(const char *n) {
	name = edelib_strndup(n, MAX_STRSZ);
}

void Entity::set_path(const char *p) {
	path = edelib_strndup(p, MAX_STRSZ);
}

void Entity::set_interface(const char *i) {
	interface = edelib_strndup(i, MAX_STRSZ);
}

void Entity::append_arg(const char *n, const char *type, ArgDirection direction, const char *access) {
	ArgSignature *s = new ArgSignature;
	s->sig = edelib_strndup(type, MAX_STRSZ);
	s->name = edelib_strndup(n, MAX_STRSZ);

	if(access) s->access = edelib_strndup(access, MAX_STRSZ);
	s->direction = direction;

	args.push_back(s);
}

bool Entity::get_prototype(char *buf, int bufsz) {
	E_RETURN_VAL_IF_FAIL(name != NULL, false);
	E_RETURN_VAL_IF_FAIL(tp != ENTITY_NONE, false);

	String ret;
	if(args.empty()) {
		ret = "void ";
		ret += name;
		ret += " ()";
	} else {
		if(tp == ENTITY_PROPERTY) {
			/* we should only get single argument */
			if(args.size() != 1) {
				E_WARNING(E_STRLOC ": Property should have only one type, but got '%i'. Using first one only...\n", args.size());
			} else {
				ArgSignatureListIt it = args.begin();
				signature_to_readable((*it)->sig, ret);

				ret += ' ';
				ret += name;

				/* write kind of access */
				if((*it)->access) {
					ret += " [access: ";
					ret += (*it)->access;
					ret += ']';
				}
			}
		} else {
			ArgSignatureListIt it = args.begin(), ite = args.end();

			/* default */
			ret = "void";

			/* return type */
			for(; it != ite; ++it) {
				if((*it)->direction == DIRECTION_OUT) {
					signature_to_readable((*it)->sig, ret);
					break;
				}
			}

			/* name */
			ret += ' ';
			ret += name;

			/* arguments */
			ret += "(";

			String tmp;
			bool   have_something = false;

			for(it = args.begin(); it != ite; ++it) {
				/* AFAIK only 'direction=out' is explicitly set */
				if((*it)->direction != DIRECTION_OUT) {
					/* so we can correctly watch adding commas */
					if(have_something) ret += ", ";

					signature_to_readable((*it)->sig, tmp);
					ret += tmp;
					ret += ' ';
					ret += (*it)->name;
					have_something = true;
				}
			}
			ret += ')';
		}
	}

	if(!ret.empty()) {
		edelib_strlcpy(buf, ret.c_str(), bufsz);
		return true;
	}

	return false;
}

bool Entity::get_prototype_as_scheme(char *buf, int bufsz) {
	E_RETURN_VAL_IF_FAIL(name != NULL, false);
	E_RETURN_VAL_IF_FAIL(tp != ENTITY_NONE, false);

	String ret;
	E_DEBUG("%i %i %s\n", tp, args.size(), get_name());

	if(tp == ENTITY_SIGNAL) {
		if(args.empty()) {
			ret.printf("(dbus-signal \"%s\" \"%s\" \"%s\")", get_path(), get_interface(), get_name());
		}
	}

	if(!ret.empty()) {
		edelib_strlcpy(buf, ret.c_str(), bufsz);
		return true;
	}

	return false;
}
