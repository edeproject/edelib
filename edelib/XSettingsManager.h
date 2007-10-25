/*
 * $Id$
 *
 * Manager part of XSETTINGS protocol
 * Part of edelib.
 * Based on implementation from Owen Tylor, copyright (c) 2001 Red Hat, inc.
 * Copyright (c) 2005-2007 EDE Authors.
 *
 * This program is licensed under terms of the 
 * GNU General Public License version 2 or newer.
 * See COPYING for details.
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
class XSettingsManager {
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
