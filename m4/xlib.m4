dnl
dnl $Id: xlib.m4 1856 2007-03-17 11:28:25Z karijes $
dnl
dnl Part of edelib
dnl Copyright (c) 2000-2007 EDE Authors.
dnl 
dnl This program is licenced under terms of the 
dnl GNU General Public Licence version 2 or newer.
dnl See COPYING for details.

AC_DEFUN([EDE_CHECK_X11], [
	dnl generic X11 checkers
	AC_PATH_X
	AC_PATH_XTRA

	dnl user does not have X installed
	dnl if we want let him build, he will explicitely must add '--without-x' option
	dnl since last one is for debugging purposes
	if eval "test $have_x = no"; then
		AC_MSG_ERROR(X11 libraries are not found! Please install them first)
	fi

	if eval "test $have_x = disabled"; then
		AC_MSG_NOTICE(*****************************************************)
		AC_MSG_NOTICE(* edelib will be built without X11 and FLTK support *)
		AC_MSG_NOTICE(*****************************************************)
	fi
])
