/*
 * $Id$
 *
 * Common stuff for XSETTINGS protocol
 * Part of edelib.
 * Based on implementation from Owen Tylor, copyright (c) 2001 Red Hat, inc.
 * Copyright (c) 2005-2007 EDE Authors.
 *
 * This program is licensed under terms of the 
 * GNU General Public License version 2 or newer.
 * See COPYING for details.
 */

#ifndef __XSETTINGSCOMMON_H__
#define __XSETTINGSCOMMON_H__

#include "econfig.h"
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
	XSettingsSetting* setting;
	XSettingsList* next;
};

/**
 * \class XSettingsData
 * \brief Main data shared between client and manager
 */
struct XSettingsData {
	Display* display;
	int screen;

	Window manager_win;
	Atom selection_atom;
	Atom manager_atom;
	Atom xsettings_atom;

	XSettingsList* settings;
	unsigned long serial;
};

/**
 * \class XSettingsBuffer
 * \brief Internal buffer used by XSETTINGS classes
 */
struct XSettingsBuffer {
	char byte_order;
	int len;
	unsigned char* data;
	unsigned char* pos;
};

/**
 * Adds setting to the current list. Only pointer will be stored in list.
 */
bool xsettings_list_add(XSettingsList** list, XSettingsSetting* setting);

/**
 * Remove setting with given name. If not found, it will do nothing.
 */
bool xsettings_list_remove(XSettingsList** list, const char* name);

/**
 * Find setting with given name. Returns NULL if nothing was found.
 */
XSettingsSetting* xsettings_list_find(XSettingsList** list, const char* name);

/**
 * Clears settings list.
 */
void xsettings_list_free(XSettingsList* list);



/**
 * Copy given setting. Copy will be allocated, and should be freed with xsettings_setting_free().
 */
XSettingsSetting* xsettings_setting_copy(XSettingsSetting* setting);

/**
 * Free data allocated by given setting.
 */
void xsettings_setting_free(XSettingsSetting* setting);

/**
 * Returns true if given two settings are equal or false if not.
 */
bool xsettings_setting_equal(XSettingsSetting* s1, XSettingsSetting* s2);

/**
 * Returns setting lenght.
 */
int  xsettings_setting_len(const XSettingsSetting* setting);

/**
 * Returns byte order (MSBFirst/LSBFirst) used on client side.
 */
char xsettings_byte_order(void);


/**
 * Decode settings from given buffer. Returns NULL if failed or buffer was empty; otherwise
 * returns list of recognised ones.
 */
XSettingsList* xsettings_decode(unsigned char* data, /*size_t*/ int len, unsigned long* serial);

/**
 * Encode settings from list into buffer.
 */
void xsettings_encode(const XSettingsSetting* setting, XSettingsBuffer* buffer);


#ifndef SKIP_DOCS
void xsettings_manager_notify(XSettingsData* data);
void xsettings_manager_set_setting(XSettingsData* data, XSettingsSetting* setting);
#endif

EDELIB_NS_END
#endif
