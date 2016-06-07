dnl
dnl $Id$
dnl
dnl Copyright (c) 2005-2011 edelib authors
dnl
dnl This library is free software; you can redistribute it and/or
dnl modify it under the terms of the GNU Lesser General Public
dnl License as published by the Free Software Foundation; either
dnl version 2 of the License, or (at your option) any later version.
dnl
dnl This library is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
dnl Lesser General Public License for more details.
dnl
dnl You should have received a copy of the GNU Lesser General Public License
dnl along with this library. If not, see <http://www.gnu.org/licenses/>.

AC_DEFUN([EDELIB_FLTK], [
	AC_ARG_WITH(fltk_path, [  --with-fltk-path=[DIR]    alternative directory where FLTK is installed])

	if test "x$with_fltk_path" = "x"; then
		AC_PATH_PROG(FLTK_CONFIG, fltk-config)
	else
		AC_PATH_PROG(FLTK_CONFIG, fltk-config,, $with_fltk_path/bin)
	fi

	if test -n "$FLTK_CONFIG"; then
		dnl test version first
		fltk_version=`$FLTK_CONFIG --version`

		AC_MSG_CHECKING([for FLTK version >= 1.1.7])
		case "$fltk_version" in ["1.1."[789]])
			AC_MSG_RESULT(yes)
			;;
			["1.1.10"])
			AC_MSG_RESULT(yes)
			;;
			dnl check for FLTK 1.3.x branch
			["1.3."[0123456789]])
			HAVE_FL_TREE=1
			AC_MSG_RESULT(yes)
			;;
			*)
			AC_MSG_ERROR([Looks like you have an older FLTK version ($fltk_version). Required is >= 1.1.7])
		esac

		dnl remove -Wno-non-virtual-dtor from flags (also, on NetBSD they added -O2 flag; stupid)
		FLTK_CFLAGS=`$FLTK_CONFIG --cxxflags | sed -e 's/-Wno-non-virtual-dtor//' | sed -e 's/-O[1-3]//g'`
		dnl remove -lsupc++ so we can chose what to use
		FLTK_LIBS=`$FLTK_CONFIG --ldflags | sed -e 's/-lsupc++//g'`
		FLTK_LIBS_FULL=`$FLTK_CONFIG --use-images --ldflags | sed -e 's/-lsupc++//g'`

		dnl check if FLTK was compiled with XFT support
		AC_MSG_CHECKING([for XFT support in FLTK])

		dnl directly grep fltk-config, instead checking for output in 'fltk-config --ldflags' as some 
		dnl linux distros (Ubuntu) implicitly links X11 libraries against FLTK
		fltk_have_xft=`cat $FLTK_CONFIG | sed -ne '/-lXft/p'`

		if test "x$fltk_have_xft" = "x"; then
			AC_MSG_RESULT(no)
		else
			AC_MSG_RESULT(yes)
			AC_DEFINE(HAVE_FLTK_XFT, 1, [Define to 1 if FLTK was compiled with XFT support])
		fi
	else
		AC_MSG_ERROR([You don't have fltk installed. To compile edelib, you will need it.])
	fi
])
