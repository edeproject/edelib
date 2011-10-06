dnl
dnl $Id: dbus.m4 2833 2009-09-24 16:38:32Z karijes $
dnl
dnl Inspired from glib varargs macros
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

AC_DEFUN([EDELIB_CPP_VARARGS], [
	AC_LANG_SAVE
	AC_LANG_CPLUSPLUS

	AC_MSG_CHECKING(for ISO cpp varargs)
	AC_TRY_COMPILE([], [
		int a(int a1, int a2, int a3);
		#define call_a(...) a(1, __VA_ARGS__);
		call_a(2,3);
	], [have_iso_varargs=yes],[have_iso_varargs=no])

	if test "x$have_iso_varargs" = "xyes"; then
		AC_MSG_RESULT(yes)
		AC_DEFINE(HAVE_ISO_VARARGS, 1, [Define to 1 if you have ISO cpp varargs])

		dnl g++ will warn for variadic macros in pedantic mode
		if test "$GCC" = yes; then
			CFLAGS="-Wno-variadic-macros $CFLAGS"
			CXXFLAGS="-Wno-variadic-macros $CXXFLAGS"
		fi
	else
		AC_MSG_RESULT(no)

		AC_MSG_CHECKING(for GNU gcc varargs)
		AC_TRY_COMPILE([], [
			int a(int a1, int a2, int a3);
			#define call_a(params...) a(1, params);
			call_a(2,3);
		], [have_gnuc_varargs=yes], [have_gnuc_varargs=no])

		if test "x$have_gnuc_varargs" = "xyes"; then
			AC_MSG_CHECKING(yes)
			AC_DEFINE(HAVE_GNUC_VARARGS, 1, [Define to 1 if you have GNU gcc varargs])
		else
			AC_MSG_CHECKING(no)
		fi
	fi

	AC_LANG_RESTORE
])
