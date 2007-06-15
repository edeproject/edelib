/*
 * $Id$
 *
 * X11 options setter
 * Part of edelib.
 * Copyright (c) 2005-2007 EDE Authors.
 *
 * This program is licenced under terms of the 
 * GNU General Public Licence version 2 or newer.
 * See COPYING for details.
 */

#include <edelib/XSet.h>
#include <X11/XKBlib.h>

EDELIB_NAMESPACE {

XSet::XSet() {
	display = XOpenDisplay("0:0");
}

XSet::XSet(const char* d) {
	display = XOpenDisplay(d);
}

XSet::~XSet() {
	if(display)
		XCloseDisplay(display);
}

void XSet::set_mouse(int accel, int thresh) {
	if(!display)
		return;
    XChangePointerControl(display, true, true, accel, 1, thresh);
}

void XSet::set_bell(int volume, int pitch, int duration) {
	if(!display)
		return;

	XKeyboardControl val;
	if(volume > 100)
		volume = 100;
	val.bell_percent = volume;
	val.bell_pitch = pitch;
	val.bell_duration = duration;
	XChangeKeyboardControl(display, KBBellPercent | KBBellPitch | KBBellDuration, &val);
}

void XSet::set_keyboard(int repeat, int clicks) {
	if(!display)
		return;

	XKeyboardControl val;
	val.auto_repeat_mode = (repeat ? AutoRepeatModeOn : AutoRepeatModeOff);
	val.key_click_percent = clicks;
	XChangeKeyboardControl(display, KBAutoRepeatMode | KBKeyClickPercent, &val);
}

void XSet::test_bell(void) {
	if(!display)
		return;

	XkbForceBell(display, 0);
	// TODO: what about XkbBell(dpy, win, volume, atom) ?
}

void XSet::test_blank(void) {
	if(!display)
		return;

	XActivateScreenSaver(display);
}

}
