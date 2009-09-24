dnl
dnl $Id$
dnl
dnl Copyright (c) 2005-2009 edelib authors
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

AC_DEFUN([EDELIB_DBUS], [
	AC_ARG_ENABLE(dbus, [  --disable-dbus          disable D-BUS support],,[enable_dbus=yes])
	if test "$enable_dbus" = yes; then

		EDELIB_PKG_CHECK_MODULES(DBUS, [dbus-1 >= 1.0], [have_dbus=yes], [have_dbus=no])
		if test "$have_dbus" = yes; then
			AC_DEFINE(HAVE_DBUS, 1, [Define to 1 if you have libdbus])
			dnl long long does not exists in current C++ standard and
			dnl gcc with '-pedantic' will report it as error; this should get rid of it
			if test "$GCC" = yes; then
				DBUS_CFLAGS="-Wno-long-long $DBUS_CFLAGS"
			fi

			dnl newer dbus versions deprecate dbus_watch_get_fd() with dbus_watch_get_unix_fd()
			AC_MSG_CHECKING([for dbus_watch_get_unix_fd()])
			CFLAGS_saved=$CFLAGS
			LIBS_saved=$LIBS

			CFLAGS="$CFLAGS $DBUS_CFLAGS"
			LIBS="$LIBS $DBUS_LIBS"

			AC_LANG_SAVE
			AC_LANG_C
			AC_TRY_LINK([
				#include <dbus/dbus.h>
			],[
				DBusWatch *ww;
				dbus_watch_get_unix_fd(ww);
			],[have_dbus_watch_get_unix_fd=yes],[have_dbus_watch_get_unix_fd=no])
			AC_LANG_RESTORE

			if test "$have_dbus_watch_get_unix_fd" = yes; then
				AC_DEFINE(HAVE_DBUS_WATCH_GET_UNIX_FD, 1, [Define to 1 if you have dbus_watch_get_unix_fd()])
				AC_MSG_RESULT(yes)
			else
				AC_MSG_RESULT(no)
			fi

			CFLAGS=$CFLAGS_saved
			LIBS=$LIBS_saved
		else
			AC_MSG_RESULT(no)
			AC_MSG_ERROR([D-Bus libraries not found! Please install them first])
		fi
	else
		DBUS_CFLAGS=""
		DBUS_LIBS=""
	fi
])
