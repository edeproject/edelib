/*
 * $Id$
 *
 * Common stuff for XSETTINGS protocol
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

#ifndef __XSETTINGSCOMMON_H__
#define __XSETTINGSCOMMON_H__

#include "edelib-global.h"
#include <X11/Xlib.h>

#define XSETTINGS_PAD(n, p) ((n + p - 1) & (~(p - 1)))

EDELIB_NS_BEGIN

/**
 * \enum XSettingsType
 * \brief Types used for XSETTINGS
 */
enum XSettingsType {
	XSETTINGS_TYPE_INT = 0,      ///< int type
	XSETTINGS_TYPE_STRING,       ///< string (char*) type
	XSETTINGS_TYPE_COLOR         ///< RGBA type
};

/**
 * \class XSettingsColor
 * \brief Color data for XSETTINGS
 */
struct XSettingsColor {
	/** red color */
	unsigned short red;
	/** green color */
	unsigned short green;
	/** blue color */
	unsigned short blue;
	/** alphacolor */
	unsigned short alpha;
};

/**
 * \class XSettingsSetting
 * \brief Structure for tracking XSETTINGS values
 */
struct XSettingsSetting {
	/** setting name */
	char* name;
	/** setting type */
	XSettingsType type;

	/** value for this setting */
	union {
		int v_int;
		char* v_string;
		XSettingsColor v_color;
	} data;

	/** serial used to check when setting was changed */
	unsigned long last_change_serial;
};

/**
 * \class XSettingsList
 * \brief List internaly used by XSETTINGS classes
 */
struct XSettingsList {
	/** current node */
	XSettingsSetting* setting;
	/** next node */
	XSettingsList* next;
};

/**
 * \class XSettingsData
 * \brief Main data shared between client and manager
 *
 * This structure is <em>shared</em> between client and manger. 
 * (should not be confused with ordinary sharing like memory sharing etc.)
 *
 * This is short explaination what are differences between official XSETTINGS protocol.
 *
 * Both client and manager implementations use this structure althought
 * some members they fill it on different ways, notably <em>manager_win</em>.
 * Manager will create and initialize <em>manager_win</em> variable but client
 * will try to figure it out if manager is running. Rest of the members are
 * filled in usual way.
 *
 * Why complicate things like this? First of all XSETTINGS protocol nicely
 * distinguish client and manager roles, where client can only read settings and
 * manager set them and keep them in one place. Main problem with this is that
 * client can <em>only</em> read those settings, but not change or alter them, so
 * if client wants to save it's preferences it has somehow communicate with manager.
 *
 * To avoid that, via current implementation, client will behave like half-manager (sorta of)
 * and it can directly write to manager space. Will this cause races? Probably, but
 * as long as client peak/write/read it's own settings problems should not occur; manager
 * will setup initial settings at the startup and will not change them during runtime (most of, for
 * me, known managers does not do this).
 *
 * Alternative would be to create some communication channel with manager, but this will create
 * incopatible manager with ones already developed. On other hand this will be the only solution
 * if we are starting to get a lot of races. 
 *
 * If someone knows better way to handle this, please mail me; I will be happy to hear alternatives.
 */
struct XSettingsData {
	/** display running XSETTINGS */
	Display* display;
	/** screen running XSETTINGS */
	int screen;

	/** manager window */
	Window manager_win;

	/** atom containing current screen number */
	Atom selection_atom;

	/** manager atom */
	Atom manager_atom;

	/** XSETTINGS atom */
	Atom xsettings_atom;

	/** list of settings we currently have */
	XSettingsList* settings;

	/** a number used to check changes */
	unsigned long serial;
};

#ifndef SKIP_DOCS
struct XSettingsBuffer {
	char byte_order;
	int len;
	unsigned char* data;
	unsigned char* pos;
};
#endif

/**
 * Adds setting to the current list. Only pointer will be stored in list.
 */
EDELIB_API bool xsettings_list_add(XSettingsList** list, XSettingsSetting* setting);

/**
 * Remove setting with given name. If not found, it will do nothing.
 */
EDELIB_API bool xsettings_list_remove(XSettingsList** list, const char* name);

/**
 * Find setting with given name. Returns NULL if nothing was found.
 */
EDELIB_API XSettingsSetting* xsettings_list_find(XSettingsList* list, const char* name);

/**
 * Clears settings list.
 */
EDELIB_API void xsettings_list_free(XSettingsList* list);



/**
 * Copy given setting. Copy will be allocated, and should be freed with xsettings_setting_free().
 */
EDELIB_API XSettingsSetting* xsettings_setting_copy(XSettingsSetting* setting);

/**
 * Free data allocated by given setting.
 */
EDELIB_API void xsettings_setting_free(XSettingsSetting* setting);

/**
 * Returns true if given two settings are equal or false if not.
 */
EDELIB_API bool xsettings_setting_equal(XSettingsSetting* s1, XSettingsSetting* s2);

/**
 * Returns setting lenght.
 */
EDELIB_API int  xsettings_setting_len(const XSettingsSetting* setting);

/**
 * Returns byte order (MSBFirst/LSBFirst) used on client side.
 */
EDELIB_API char xsettings_byte_order(void);


/**
 * Decode settings from given buffer. Returns NULL if failed or buffer was empty; otherwise
 * returns list of recognised ones.
 */
EDELIB_API XSettingsList* xsettings_decode(unsigned char* data, /*size_t*/ int len, unsigned long* serial);

/**
 * Encode settings from list into buffer.
 */
EDELIB_API void xsettings_encode(const XSettingsSetting* setting, XSettingsBuffer* buffer);


#ifndef SKIP_DOCS
void xsettings_manager_notify(XSettingsData* data);
void xsettings_manager_set_setting(XSettingsData* data, XSettingsSetting* setting);
void xsettings_manager_delete_setting(XSettingsData* data, const char* name);
#endif

EDELIB_NS_END
#endif
