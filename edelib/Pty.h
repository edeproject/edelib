/*
 * $Id$
 *
 * A class for handling pseudoterminals (PTYs)
 * Copyright (c) 2006-2008 edelib authors
 *
 * This file was a part of the KDE project, module kdesu.
 * Copyright (C) 1999,2000 Geert Jansen <jansen@kde.org>
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

#ifndef __EDELIB_PTY_H__
#define __EDELIB_PTY_H__

#include "edelib-global.h"

EDELIB_NS_BEGIN

/**
 * \class PTY
 * \brief emulate pty api
 *
 * PTY compatibility routines. This class tries to emulate a UNIX98 PTY API
 * on various platforms.
 */
class EDELIB_API PTY {
private:
	int ptyfd;
	char *ptyname, *ttyname;

	E_DISABLE_CLASS_COPY(PTY)
public:
	/**
	 * Construct a PTY object
	 */
	PTY();

	/**
	 * Destructs the object. The PTY is closed if it is still open
	 */
	~PTY();

	/**
	 * Allocate a pty
	 * \return A filedescriptor to the master side
	 */
	int getpt();

	/**
	 * Grant access to the slave side
	 * \return zero if succesfull, < 0 otherwise
	 */
	int grantpt();

	/**
	 * Unlock the pty. This allows connections on the slave side.
	 * \return Zero if successful, < 0 otherwise
	 */
	int unlockpt();

	/**
	 * Return the slave side name
	 */
	const char *ptsname();
};

EDELIB_NS_END
#endif
