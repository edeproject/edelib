/*
 * $Id$
 *
 * Icon choose dialog
 * Part of edelib.
 * Copyright (c) 2000-2007 EDE Authors.
 *
 * This program is licenced under terms of the
 * GNU General Public Licence version 2 or newer.
 * See COPYING for details.
 */

#ifndef __ICONCHOOSER_H__
#define __ICONCHOOSER_H__

#include "econfig.h"
#include "String.h"

EDELIB_NAMESPACE {

/**
 * icon_chooser() is a dialog containing a list of visible
 * icons so user can choose desired one. Given directory
 * will be scanned for known image/icon types and those will
 * be shown.
 *
 * If given path does not exists, or directory is not readable,
 * empty dialog will be shown.
 *
 * \image html icon_chooser.jpg
 *
 * \note icon_chooser() will skip those icons who's size
 * is grater than 128 pixels, width or height.
 *
 * \return full path to choosed icon, or empty string if directory is inaccessible, or pressed Cancel
 * \param dir a path to directory that contains icons
 */

EDELIB_API String icon_chooser(const char* dir);

}
#endif
