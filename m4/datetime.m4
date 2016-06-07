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

dnl Date/time functions checks
AC_DEFUN([EDELIB_DATETIME], [
	AC_CHECK_FUNC(gmtime_r, [have_gmtime_r=yes], [have_gmtime_r=no])
	if test "$have_gmtime_r" = yes; then
		AC_DEFINE(HAVE_GMTIME_R, 1, [Define to 1 if you have gmtime_r()])
	fi

	AC_CHECK_FUNC(localtime_r, [have_localtime_r=yes], [have_localtime_r=no])
	if test "$have_localtime_r" = yes; then
		AC_DEFINE(HAVE_LOCALTIME_R, 1, [Define to 1 if you have localtime_r()])
	fi

	AC_CHECK_FUNC(settimeofday, [have_settimeofday=yes], [have_settimeofday=no])
	if test "$have_settimeofday" = yes; then
		AC_DEFINE(HAVE_SETTIMEOFDAY, 1, [Define to 1 if you have settimeofday()])
	fi

	AC_CHECK_FUNC(stime, [have_stime=yes], [have_stime=no])
	if test "$have_stime" = yes; then
		AC_DEFINE(HAVE_STIME, 1, [Define to 1 if you have stime()])
	fi

	dnl Check for daylight variable in time.h. FreeBSD (possible other BSDs don't have it)
	AC_MSG_CHECKING([for daylight variable])
	AC_LANG_SAVE
	AC_LANG_C
	AC_TRY_LINK([
		#include <time.h>
	],[
		extern int daylight;
		int d = daylight;
	],[have_daylight=yes],[have_daylight=no])
	AC_LANG_RESTORE

	if test "$have_daylight" = yes; then
		AC_DEFINE(HAVE_DAYLIGHT, 1, [Define to 1 if you have daylight variable])
		AC_MSG_RESULT(yes)
	else
		AC_MSG_RESULT(no)
	fi
])
