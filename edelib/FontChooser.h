/*
 * $Id$
 *
 * Font chooser
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

#ifndef __EDELIB_FONTCHOOSER_H__
#define __EDELIB_FONTCHOOSER_H__

#include "edelib-global.h"

EDELIB_NS_BEGIN

/**
 * \ingroup widgets
 *
 * font_chooser() is a dialog for choosing fonts. After this function
 * was called, it will display a dialog with available fonts and their
 * sizes and will wait untill user choose an action.
 *
 * If user canceled it (or it was closed) it will return -1 and <em>retsize</em>
 * will be set to -1.
 *
 * Note that dialog returns (besides font size), integer value for font name,
 * so it can be directly used by FLTK. Returned value is not valuable for end user
 * (e.g. when you want to display it in some dialog), so you must use 
 * Fl::get_font_name((Fl_Font)returned_value, 0) to obtain stringized name (e.g. helvetica, courier, etc.)
 *
 * \return font name as integer, known to FLTK
 * \param name is titlebar
 * \param family is family of fonts (like ISO8859-1). If given NULL, it will use ISO8859-1; if given
 * <i>-*</i> will get all fonts with any encoding as long as they have normal X font names with dashes in them;
 * if given <i>*</i> will get every font that exists.
 * \param retsize if not NULL will return selected font size
 * \param default_name if not NULL will select given font name
 * \param default_size if greater than 0 will select font size
 */
EDELIB_API int font_chooser(const char* name, const char* family, int& retsize, const char* default_name = 0, int default_size = 0);

EDELIB_NS_END
#endif
