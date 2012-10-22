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

dnl inotify checks (linux)
AC_DEFUN([__INOTIFY_CHECK], [
	AC_MSG_CHECKING([for inotify presence])

	AC_LANG_SAVE
	AC_LANG_C
	AC_TRY_COMPILE([
		#include <sys/inotify.h>
		#include <unistd.h>
	],[
		inotify_init();
	],[have_inotify=yes],[have_inotify=no])
	AC_LANG_RESTORE

	if test "$have_inotify" = yes; then
		AC_DEFINE(HAVE_INOTIFY, 1, [Define to 1 if you have inotify support in the kernel (Linux only)])
		AC_MSG_RESULT(yes)
	else
		AC_MSG_RESULT(no)
	fi
])

dnl dnotify checks (linux)
dnl not used
AC_DEFUN([__DNOTIFY_CHECK], [
	AC_MSG_CHECKING([for dnotify presence])

	dnl stolen from kde
	AC_LANG_SAVE
	AC_LANG_C
	AC_TRY_COMPILE([
		#ifndef _GNU_SOURCE
		#define _GNU_SOURCE
		#endif
		#include <fcntl.h>
		#include <signal.h>
	],[
		#ifndef F_NOTIFY
		#error no dnotify
		#endif
		int fd;
		fcntl(fd, F_SETSIG, SIGRTMIN);
		fcntl(fd, F_NOTIFY, DN_CREATE|DN_MULTISHOT);
	],[have_dnotify=yes],[have_dnotify=no])
	AC_LANG_RESTORE
		
	if test "$have_dnotify" = yes; then
		AC_DEFINE(HAVE_DNOTIFY, 1, [Define to 1 if you have dnotify support in the kernel (Linux only)])
		AC_MSG_RESULT(yes)
	else
		AC_MSG_RESULT(no)
	fi
])

dnl kqueue (BSDs) checks
dnl not used
AC_DEFUN([__KQUEUE_CHECK], [
	AC_MSG_CHECKING([for kqueue presence])

	AC_LANG_SAVE
	AC_LANG_C
	AC_TRY_COMPILE([
		#include <sys/types.h>
		#include <sys/event.h>
		#include <sys/time.h>
	],[
		kqueue();
	],[have_kqueue=yes],[have_kqueue=no])
	AC_LANG_RESTORE

	if test "$have_kqueue" = yes; then
		AC_DEFINE(HAVE_KQUEUE, 1, [Define to 1 if you have kqueue support in the kernel (BSDs only)])
		AC_MSG_RESULT(yes)
	else
		AC_MSG_RESULT(no)
	fi
])

dnl FAM checks
AC_DEFUN([__FAM_CHECK], [
	AC_MSG_CHECKING([for FAM presence])

	AC_LANG_SAVE
	AC_LANG_C
	AC_TRY_COMPILE([
		#include <fam.h>
	],[
		FAMEvent event;
	],[have_fam=yes],[have_fam=no])
	AC_LANG_RESTORE

	if test "$have_fam" = yes; then
		AC_DEFINE(HAVE_FAM, 1, [Define to 1 if you have FAM installed])
		AC_MSG_RESULT(yes)

		FAMLIB="-lfam"

		dnl check do FAM have FAMNoExists function (gamin only)
		tmp_libs="$LIBS"
		LIBS="$LIBS $FAMLIB"
		AC_CHECK_FUNC([FAMNoExists], [have_famnoexists=yes], [have_famnoexists=no])
		if test "$have_famnoexists" = yes; then
			AC_DEFINE(HAVE_FAMNOEXISTS, 1, [Define to 1 if you have FAMNoExists in FAM implementation])
		fi

		LIBS="$tmp_libs"
	else
		AC_MSG_RESULT(no)
	fi

])

dnl Checks for the file system notification support.
dnl First will be checked for inotify, then as fallback for FAM/Gamin
dnl if was given --enable-fam-only, only FAM/Gamin will be used
AC_DEFUN([EDELIB_NOTIFY], [
	AC_ARG_ENABLE(fam_only, AC_HELP_STRING([--enable-fam-only], [use FAM regardless of what kernel-level systems are available (default=no)]),
		[enable_fam_only=yes], [enable_fam_only=no])

	dnl this must be global since 'have_fam' is visible only inside if/fi block
	notify_have_fam=no

	if test "$enable_fam_only" = yes; then
		__FAM_CHECK
		notify_have_fam=$have_fam
	else
		dnl make sure none of the kernel notifiers are available before
		dnl FAM fallback so we don't link code with libfam
		__INOTIFY_CHECK

		if test "$have_inotify" = no; then
			__FAM_CHECK
			notify_have_fam=$have_fam
		fi
	fi

	dnl FAMLIB will be set only when tests for FAM are passed
	if test "$notify_have_fam" = yes; then
		LIBFAM_LIBS="$FAMLIB"
	fi
])
