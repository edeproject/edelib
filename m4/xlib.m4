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

AC_DEFUN([EDELIB_X11], [
	AC_PATH_XTRA

	dnl user does not have X installed
	dnl if we want let him build, he will explicitely must add '--without-x' option
	dnl since last one is for debugging purposes
	if test "$have_x" = no; then
		AC_MSG_ERROR(X11 libraries are not found! Please install them first)
	fi

	if test "$have_x" = disabled; then
		AC_MSG_NOTICE(*****************************************************)
		AC_MSG_NOTICE(* edelib will be built without X11 and FLTK support *)
		AC_MSG_NOTICE(*****************************************************)
	else
		dnl see if we can use C++ with X libs (long long issue on some platforms)
		ac_saved_CXXFLAGS="$CXXFLAGS"
		ac_saved_LIBS="$LIBS"

		AC_LANG_SAVE
		AC_LANG_CPLUSPLUS

		# to detect long long warning, must use compiler pedantic option
		if test "x$GCC" = "xyes"; then
			pedantic_flag="-pedantic"
		fi

		CXXFLAGS="$X_CFLAGS $pedantic_flag"
		LIBS="$X_LIBS $X_EXTRA_LIBS"

		AC_TRY_COMPILE([
#include <X11/Xproto.h>
#include <X11/Xmd.h>
],[
  /* nothing */
],[xlib_cpp_longlong=yes],[xlib_cpp_longlong=no])

		if test "x$xlib_cpp_longlong" = "xno"; then
			if test "x$GCC" = "xyes"; then
				wno_long_long="-Wno-long-long"
			fi
		fi

		AC_LANG_RESTORE

		CXXFLAGS="$wno_long_long $ac_saved_CXXFLAGS"
		LIBS="$ac_saved_LIBS"
	fi
])
