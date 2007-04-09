dnl
dnl $Id: fltk.m4 1856 2007-03-17 11:28:25Z karijes $
dnl
dnl Part of edelib.
dnl Copyright (c) 2000-2007 EDE Authors.
dnl 
dnl This program is licenced under terms of the 
dnl GNU General Public Licence version 2 or newer.
dnl See COPYING for details.

AC_DEFUN([EDE_CHECK_FLTK], [
	dnl AC_MSG_NOTICE(whether is fltk 2.0 present)
	AC_PATH_PROG(FLTK2_CONFIG, fltk2-config)
	if test -n "$FLTK2_CONFIG"; then
		dnl remove -Wno-non-virtual-dtor from flags
		FLTKFLAGS=`$FLTK2_CONFIG --cxxflags | sed -e 's/-Wno-non-virtual-dtor//'`

		dnl remove -lsupc++ so we can chose what to use
		FLTKLIBS=`$FLTK2_CONFIG --use-images --ldflags | sed -e 's/-lsupc++//g'`
	else
		AC_MSG_ERROR([You don't have fltk2 installed. To compile edelib, you will need it.])
	fi
])
