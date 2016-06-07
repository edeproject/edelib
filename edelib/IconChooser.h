/*
 * $Id$
 *
 * Icon chooser
 * Copyright (c) 2005-2007 edelib authors
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

#ifndef __EDELIB_ICONCHOOSER_H__
#define __EDELIB_ICONCHOOSER_H__

#include "String.h"
#include "IconTheme.h"

EDELIB_NS_BEGIN

/**
 * \ingroup widgets
 *
 * icon_chooser() is a dialog containing a list of visible icons so user can choose desired one. Given directory
 * will be scanned for known image/icon types and those will be shown.
 *
 * If given path does not exists, or directory is not readable, empty dialog will be shown.
 *
 * \image html icon_chooser.jpg
 *
 * \note icon_chooser() will skip those icons who's size is greater than 128 pixels, width or height.
 *
 * \return full path to choosed icon, or empty string if directory is inaccessible, or pressed Cancel
 * \param dir a path to directory that contains icons
 */
EDELIB_API String icon_chooser(const char *dir);

/**
 * \ingroup widgets
 *
 * Load dialog with icons from currently loaded icon theme. This function will assume <em>IconTheme::init(...)</em>
 * was previously called and (if not) it will show empty dialog. If user select icon inside icon theme, returned value
 * will be icon name base: only file name without folder prefix and extension.
 *
 * However, if user select icon outside icon theme, it will return full path. This is because IconTheme loader can also load
 * icons if was given full path to icon file.
 *
 * \return full path to choosed icon or empty string if theme was not loaded or pressed Cancel
 * \param sz is icon size
 * \param ctx is icon context
 * \param always_full_path if set, icons will not be stripped
 * \param outside_theme_ret if variable was given, returns true if icon was selected inside or outside icon theme
 */
EDELIB_API String icon_chooser(IconSizes sz, IconContext ctx = ICON_CONTEXT_ANY, bool always_full_path = false, bool *outside_theme_ret = NULL);

EDELIB_NS_END
#endif
