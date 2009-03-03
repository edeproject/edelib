dnl
dnl $Id$
dnl
dnl Copyright (c) 2005-2007 edelib authors
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
