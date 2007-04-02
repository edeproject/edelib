/*
 * $Id$
 *
 * Message dialogs
 * Part of edelib.
 * Copyright (c) 2005-2007 EDE Authors.
 *
 * This program is licenced under terms of the 
 * GNU General Public Licence version 2 or newer.
 * See COPYING for details.
 */

#ifndef __ASK_H__
#define __ASK_H__

#include "econfig.h"

EDELIB_NAMESPACE {

void message(const char* fmt, ...);
void alert(const char* fmt, ...);
int  ask(const char* fmt, ...);
const char *input(const char *label, const char *deflt = 0, ...);
const char *password(const char *label, const char *deflt = 0, ...);

}

#endif
