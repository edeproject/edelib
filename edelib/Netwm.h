/*
 * $Id$
 *
 * Functions for easier communication with window manager
 * Copyright (c) 2009 edelib authors
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

#ifndef __EDELIB_NETWM_H__
#define __EDELIB_NETWM_H__

#include "edelib-global.h"
#include <X11/Xlib.h>

EDELIB_NS_BEGIN

/**
 * \defgroup wm Functions for easier talk with window managers
 */

/**
 * \ingroup wm
 * Various codes reported to callback added with netwm_callback_add().
 */
enum {
	NETWM_CHANGED_WORKSPACE_COUNT,       ///< workspace count was changed
	NETWM_CHANGED_WORKSPACE_NAMES,       ///< workspace names gets changed
	NETWM_CHANGED_CURRENT_WORKSPACE,     ///< different workspace was activated
	NETWM_CHANGED_CURRENT_WORKAREA,      ///< workaread was changed
	NETWM_CHANGED_ACTIVE_WINDOW,         ///< different window was selected
	NETWM_CHANGED_WINDOW_NAME,           ///< window name was changed
	NETWM_CHANGED_WINDOW_VISIBLE_NAME,   ///< window visible name was changed
	NETWM_CHANGED_WINDOW_DESKTOP,        ///< window is moved to another workspace
	NETWM_CHANGED_WINDOW_LIST            ///< window list was changed (another window is created or existing was closed)
};

/**
 * \ingroup wm
 * Window type codes. Codes marked as <em>(part of 1.4)</em> are added in Window Manager Specification 1.4
 * and not all window managers support it. The rest of codes arer part of 1.3 specification version.
 */
enum {
	NETWM_WINDOW_TYPE_NORMAL,            ///< ordinary window
	NETWM_WINDOW_TYPE_DESKTOP,           ///< window placed as desktop
	NETWM_WINDOW_TYPE_DOCK,              ///< docked window, like pager or panel
	NETWM_WINDOW_TYPE_TOOLBAR,           ///< window as toolbar
	NETWM_WINDOW_TYPE_MENU,              ///< window as menu
	NETWM_WINDOW_TYPE_UTILITY,           ///< window as utility window
	NETWM_WINDOW_TYPE_SPLASH,            ///< splash window
	NETWM_WINDOW_TYPE_DIALOG,            ///< dialog window

	NETWM_WINDOW_TYPE_DROPDOWN_MENU,     ///< dropdown menu window (part of 1.4)
	NETWM_WINDOW_TYPE_POPUP_MENU,        ///< popup window (part of 1.4)
	NETWM_WINDOW_TYPE_TOOLTIP,           ///< tooltip (part of 1.4)
	NETWM_WINDOW_TYPE_NOTIFICATION,      ///< notification (part of 1.4)
	NETWM_WINDOW_TYPE_COMBO,             ///< window popped up by combo boxes (part of 1.4)
	NETWM_WINDOW_TYPE_DND                ///< window is being dragged (part of 1.4)
};

/**
 * \ingroup wm
 * State of the window.
 */
enum WmStateValue {
	WM_STATE_NONE      = -1,   ///< window state couldn't be determined
	WM_STATE_WITHDRAW  = 0,    ///< withdraw state
	WM_STATE_NORMAL    = 1,    ///< window is visible
	WM_STATE_ICONIC    = 3     ///< window is hidden
};

/**
 * \ingroup wm
 * Callback type for netwm_callback_add().
 */
typedef void (*NetwmCallback)(int action, Window xid, void *data);

/**
 * \ingroup wm
 * Register callback for events related to windows and environment. This function will open display
 * by calling fl_open_display().
 */
void netwm_callback_add(NetwmCallback cb, void *data = 0);

/**
 * \ingroup wm
 * Remove callback if exists.
 */
void netwm_callback_remove(NetwmCallback cb);

/**
 * \ingroup wm
 * Get current workaread set by window manager. Return false if fails.
 */
bool netwm_get_workarea(int& x, int& y, int& w, int &h);

/**
 * \ingroup wm
 * Set window type by using one of NETWM_WINDOW_TYPE_* codes. Depending on window managers some types
 * will be applied correctly when window is showed and for some, application must be restarted.
 */
void netwm_set_window_type(Window win, int t);

/**
 * \ingroup wm
 * Resize area by setting offsets to each side. <em>win</em> will be outside that area.
 */
void netwm_set_window_strut(Window win, int left, int right, int top, int bottom);

/**
 * \ingroup wm
 * Return number of available workspaces. Returns -1 if failed to fetch workspaces count.
 */
int netwm_get_workspace_count(void);

/**
 * \ingroup wm
 * Change current workspace.
 */
void netwm_change_workspace(int n);

/** 
 * \ingroup wm
 * Currently visible workspace. Workspaces are starting from 0. Returns -1 if failed.
 */
int netwm_get_current_workspace(void);

/**
 * \ingroup wm
 * Get array of workspace names. Function returns number of allocated items in array and should
 * be freed with XFreeStringList(names).
 */
int netwm_get_workspace_names(char**& names);

/**
 * \ingroup wm
 * Get array of mapped windows. Returns array size or -1 if fails. Call XFree() to free allocation.
 */
int netwm_get_mapped_windows(Window **windows);

/**
 * \ingroup wm
 * Get workspace given window resides. If fails returns -2 or return -1 if window is sticky (present on
 * all workspaces). */
int netwm_get_window_workspace(Window win);

/** 
 * \ingroup wm
 * Return 1 if given window is manageable (window can be moved or closed) or 0 if not. Desktop/dock/splash 
 * types are not manageable. In case it could not figure out this property, it will return -1.
 */
int netwm_manageable_window(Window win);

/**
 * \ingroup wm
 * Return window title or NULL if fails. Call free() on returned string.
 */
char *netwm_get_window_title(Window win);

/**
 * \ingroup wm
 * Return ID of currently focused window. If fails, return -1.
 */
Window netwm_get_active_window(void);

/**
 * \ingroup wm
 * Try to focus or raise given window.
 */
void netwm_set_active_window(Window win);

/**
 * \ingroup wm
 * Maximize window.
 */
void netwm_maximize_window(Window win);

/**
 * \ingroup wm
 * Close window.
 */
void netwm_close_window(Window win);

/**
 * \ingroup wm
 * edewm specific: restore window to previous state
 */
void wm_ede_restore_window(Window win);

/**
 * \ingroup wm
 * Not part of NETWM. Set window state to one of WmStateValue.
 */
WmStateValue wm_get_window_state(Window win);

/**
 * \ingroup wm
 * Not part of NETWM. Get one of WmStateValue for given window.
 */
void wm_set_window_state(Window win, WmStateValue state);

EDELIB_NS_END
#endif
