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

#ifndef __EDELIB_DBUS_EXPLORER_DEFAULT_H__
#define __EDELIB_DBUS_EXPLORER_DEFAULT_H__

/* some default values shared between code */

/*
 * standard value when method/signal calls is written, with argumens; also used by
 * editor to find next unreplaced word
 */
#define EDELIB_DBUS_EXPLORER_DEFAULT_VALUE_TEMPLATE "REPLACE_ME"


/*
 * The size of eval buffer for scheme; bigger means scheme will write more output in it.
 * This has nothing to do with ScriptEditor buffer, which will append content of scheme
 * evaliation buffer, limited with this size.
 */
#define EDELIB_DBUS_EXPLORER_DEFAULT_SCRIPT_EVAL_BUFSIZE 1024

#endif
