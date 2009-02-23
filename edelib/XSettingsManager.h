/*
 * $Id$
 *
 * Manager part of XSETTINGS protocol
 * Based on implementation from Owen Tylor, copyright (c) 2001 Red Hat, inc.
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

#ifndef __XSETTINGSMANAGER_H__
#define __XSETTINGSMANAGER_H__

#include "XSettingsCommon.h"

EDELIB_NS_BEGIN

/**
 * \class XSettingsManager
 * \brief Manager part of XSETTINGS protocol
 *
 * XSettingsManager is class used to make manager(s) for XSETTINGS. Please note that
 * XSETTINGS supports <em>only one</em> manager to be up so this must be taken into account.
 *
 * Final applications should not use this class except manager is not provided.
 */
class EDELIB_API XSettingsManager {
protected:
	/** Data used by manager class */
	XSettingsData* manager_data;

public:
	/**
	 * Empty constructor
	 */
	XSettingsManager();

	/**
	 * Clears internal data
	 */
	~XSettingsManager();

	/**
	 * Init needed communication parts.
	 * \return false if failed
	 * \param dpy is display
	 * \param screen is screen
	 */
	bool init(Display* dpy, int screen);

	/**
	 * Clears internal data. init() must be called agan if other
	 * functions are going to be called
	 */
	void clear(void);

	/**
	 * Checks if other manager is running
	 * \return true if does otherwise false
	 * \param dpy is display
	 * \param screen is screen
	 */
	static bool manager_running(Display* dpy, int screen);

	/**
	 * This function must be placed in X loop
	 * \returns true if should terminate
	 * \param xev is X event
	 */
	bool should_terminate(const XEvent* xev);

	/**
	 * Set integer value on manager
	 * \param name is setting name
	 * \param val is int value
	 */
	void set(const char* name, int val);

	/**
	 * Set string (char*) value on manager. Data will be copied
	 * \param name is setting name
	 * \param val is string value
	 */
	void set(const char* name, const char* val);

	/**
	 * Set RGBA color values on manager
	 * \param name is setting name
	 * \param red is red component
	 * \param green is green component
	 * \param blue is blue component
	 * \param alpha is alpha component
	 */
	void set(const char* name, unsigned short red, unsigned short green, unsigned short blue, unsigned short alpha);

	/**
	 * Notify for changed data. This should be called aftear set()
	 */
	void notify(void);
};

EDELIB_NS_END

#endif
