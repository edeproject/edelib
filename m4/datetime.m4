dnl
dnl $Id$
dnl
dnl Part of edelib.
dnl Copyright (c) 2000-2007 EDE Authors.
dnl 
dnl This program is licenced under terms of the 
dnl GNU General Public Licence version 2 or newer.
dnl See COPYING for details.

dnl Date/time functions checks
AC_DEFUN([EDELIB_DATETIME], [
	AC_CHECK_FUNC(gmtime_r, [have_gmtime_r=yes], [have_gmtime_r=no])
	if eval "test $have_gmtime_r = yes"; then
		AC_DEFINE(HAVE_GMTIME_R, 1, [Define to 1 if you have gmtime_r()])
	fi

	AC_CHECK_FUNC(localtime_r, [have_localtime_r=yes], [have_localtime_r=no])
	if eval "test $have_localtime_r = yes"; then
		AC_DEFINE(HAVE_LOCALTIME_R, 1, [Define to 1 if you have localtime_r()])
	fi

	AC_CHECK_FUNC(settimeofday, [have_settimeofday=yes], [have_settimeofday=no])
	if eval "test $have_settimeofday = yes"; then
		AC_DEFINE(HAVE_SETTIMEOFDAY, 1, [Define to 1 if you have settimeofday()])
	fi

	AC_CHECK_FUNC(stime, [have_stime=yes], [have_stime=no])
	if eval "test $have_stime = yes"; then
		AC_DEFINE(HAVE_STIME, 1, [Define to 1 if you have stime()])
	fi
])
