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

#ifndef __EDELIB_DBUS_EXPLORER_SCRIPT_DBUS_H__
#define __EDELIB_DBUS_EXPLORER_SCRIPT_DBUS_H__

#include <edelib/EdbusConnection.h>
#include <edelib/String.h>

/* scheme bindings for DBus */
struct scheme;
class  ScriptEditor;

void script_dbus_load(scheme *s, EDELIB_NS_PREPEND(EdbusConnection) **con);
void script_dbus_setup_help(scheme *s, ScriptEditor *editor);

/* so we can query 'readable' value without duplicating the code */
bool script_dbus_get_property_value(scheme *s,
									const char *service,
									const char *object,
									const char *interface,
									const char *prop,
									EDELIB_NS_PREPEND(String) &ret);
#endif


