dnl
dnl $Id$
dnl
dnl Part of edelib.
dnl Copyright (c) 2000-2007 EDE Authors.
dnl 
dnl This program is licenced under terms of the 
dnl GNU General Public Licence version 2 or newer.
dnl See COPYING for details.

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

	if eval "test $have_inotify = yes"; then
		AC_DEFINE(HAVE_INOTIFY, 1, [Define to 1 if you have inotify support in the kernel (Linux only)])
		AC_MSG_RESULT(yes)
	else
		AC_MSG_RESULT(no)
	fi
])

dnl dnotify checks (linux)
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
		
	if eval "test $have_dnotify = yes"; then
		AC_DEFINE(HAVE_DNOTIFY, 1, [Define to 1 if you have dnotify support in the kernel (Linux only)])
		AC_MSG_RESULT(yes)
	else
		AC_MSG_RESULT(no)
	fi
])

dnl kqueue (BSDs) checks
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

	if eval "test $have_kqueue = yes"; then
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

	if eval "test $have_fam = yes"; then
		AC_DEFINE(HAVE_FAM, 1, [Define to 1 if you have FAM installed])
		AC_MSG_RESULT(yes)
	else
		AC_MSG_RESULT(no)
	fi
])

dnl Checks for the file system notification support.
dnl First will be checked for inotify, then for dnotify, then for kqueue (BSDs)
dnl and at last for fam/gamin
AC_DEFUN([EDELIB_NOTIFY], [
	__INOTIFY_CHECK
	__DNOTIFY_CHECK
	__KQUEUE_CHECK
	__FAM_CHECK
])
