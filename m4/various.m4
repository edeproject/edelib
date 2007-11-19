dnl
dnl $Id: various.m4 1856 2007-03-17 11:28:25Z karijes $
dnl
dnl Part of edelib.
dnl Copyright (c) 2000-2007 EDE Authors.
dnl 
dnl This program is licenced under terms of the 
dnl GNU General Public Licence version 2 or newer.
dnl See COPYING for details.

dnl --enable-debug and --enable-profile options
AC_DEFUN([EDELIB_DEVELOPMENT], [
	AC_ARG_ENABLE(debug, [  --enable-debug          enable debug],,enable_debug=no)
	if eval "test $enable_debug = yes"; then
		DEBUGFLAGS="$DEBUGFLAGS -g3"
		dnl clear all optimization flags
		OPTIMFLAGS=""
	fi

	AC_ARG_ENABLE(profile, [  --enable-profile        enable profile],,enable_profile=no)
	if eval "test $enable_profile = yes"; then
		DEBUGFLAGS="$DEBUGFLAGS -pg"
		dnl clear all optimization flags
		OPTIMFLAGS=""
	fi
])
