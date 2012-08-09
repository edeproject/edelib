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

#ifndef __EDELIB_DBUS_EXPLORER_OBJECT_TREE_H__
#define __EDELIB_DBUS_EXPLORER_OBJECT_TREE_H__

#include <FL/Fl_Tree.H>
#include <edelib/EdbusConnection.h>
#include <edelib/List.h>

class Entity;

typedef EDELIB_NS_PREPEND(list<Entity*>) EntityList;
typedef EDELIB_NS_PREPEND(list<Entity*>::iterator) EntityListIt;

class ObjectTree : public Fl_Tree {
private:
	EntityList entities;
public:
	ObjectTree(int X, int Y, int W, int H, const char *l = 0);
	virtual ~ObjectTree() { clear(); }
	void introspect(const char *service, EDELIB_NS_PREPEND(EdbusConnection) *c);
	void append_entity(Entity *e) { entities.push_back(e); }
	void clear(void);
};

#endif
