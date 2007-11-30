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
 * \todo Finish documentation
 */
int font_chooser(const char* name, const char* family, int& retsize);

EDELIB_NS_END
