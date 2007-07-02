dnl
dnl $Id$
dnl
dnl Part of edelib.
dnl Copyright (c) 2000-2007 EDE Authors.
dnl 
dnl This program is licenced under terms of the 
dnl GNU General Public Licence version 2 or newer.
dnl See COPYING for details.

dnl Check do we have dnotify enabled in kernel
AC_DEFUN([EDE_DNOTIFY], [
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
		AC_DEFINE(HAVE_DNOTIFY, 1, [Define to 1 if you want dnotify support])
		AC_MSG_RESULT(yes)
	else
		AC_MSG_RESULT(no)
	fi
])
