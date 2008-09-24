dnl
dnl $Id: fltk.m4 1856 2007-03-17 11:28:25Z karijes $
dnl
dnl Part of edelib.
dnl Copyright (c) 2006-2008 EDE Authors.
dnl 
dnl This program is licenced under terms of the 
dnl GNU General Public Licence version 2 or newer.
dnl See COPYING for details.

AC_DEFUN([EDELIB_CHECK_FLTK], [
	AC_ARG_WITH(fltk_path, [  --with-fltk-path=[DIR]    alternative directory where FLTK is installed])

	if test "x$with_fltk_path" = "x"; then
		AC_PATH_PROG(FLTK_CONFIG, fltk-config,)
	else
		AC_PATH_PROG(FLTK_CONFIG, fltk-config,, $with_fltk_path/bin)
	fi

	if test -n "$FLTK_CONFIG"; then
		dnl test version first
		fltk_version=`$FLTK_CONFIG --version`

		AC_MSG_CHECKING([for FLTK version >= 1.1.7])
		case "$fltk_version" in ["1.1."[789]])
			dnl Display 'yes' for fltk version check
			AC_MSG_RESULT(yes)
			;;
			*)
			AC_MSG_ERROR([Looks like you have an older FLTK version ($fltk_version). Required is >= 1.1.7])
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
