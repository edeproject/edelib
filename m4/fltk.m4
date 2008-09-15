dnl
dnl $Id: fltk.m4 1856 2007-03-17 11:28:25Z karijes $
dnl
dnl Part of edelib.
dnl Copyright (c) 2000-2007 EDE Authors.
dnl 
dnl This program is licenced under terms of the 
dnl GNU General Public Licence version 2 or newer.
dnl See COPYING for details.

AC_DEFUN([EDELIB_CHECK_FLTK], [
	dnl AC_MSG_NOTICE(whether is fltk (minimum 1.1.7) present)
	AC_PATH_PROG(FLTK_CONFIG, fltk-config)
	if test -n "$FLTK_CONFIG"; then
		dnl test version first
		FLTKVERSION=`$FLTK_CONFIG --version`

		AC_MSG_CHECKING([for FLTK version >= 1.1.7])
		case "$FLTKVERSION" in ["1.1."[789]])
			dnl Display 'yes' for fltk version check
			AC_MSG_RESULT(yes)
			;;
			*)
			AC_MSG_ERROR([Looks like you have an older FLTK version ($FLTKVERSION). Required is >= 1.1.7])
		esac

		dnl remove -Wno-non-virtual-dtor from flags
		FLTK_CFLAGS=`$FLTK_CONFIG --cxxflags | sed -e 's/-Wno-non-virtual-dtor//'`
		dnl remove -lsupc++ so we can chose what to use
		FLTK_LIBS=`$FLTK_CONFIG --ldflags | sed -e 's/-lsupc++//g'`
		FLTK_LIBS_FULL=`$FLTK_CONFIG --use-images --ldflags | sed -e 's/-lsupc++//g'`
	else
		AC_MSG_ERROR([You don't have fltk installed. To compile edelib, you will need it.])
	fi
])
