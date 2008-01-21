/*
 * $Id$
 *
 * Font chooser
 * Part of edelib.
 * Copyright (c) 2005-2007 EDE Authors.
 *
 * This program is licensed under terms of the 
 * GNU General Public License version 2 or newer.
 * See COPYING for details.
 */

#include "econfig.h"

EDELIB_NS_BEGIN

/**
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
 * \return font name as integere, known to FLTK
 * \param family is family of fonts (like ISO8859-1). If given NULL, it will use ISO8859-1; if given
 * <i>-*</i> will get all fonts with any encoding as long as they have normal X font names with dashes in them;
 * if given <i>*</i> will get every font that exists.
 * \param retsize if not NULL will return selected font size
 * \param default_name if not NULL will select given font name
 * \param default_size if greater than 0 will select font size
 */
int font_chooser(const char* name, const char* family, int& retsize, const char* default_name = 0, int default_size = 0);

EDELIB_NS_END
