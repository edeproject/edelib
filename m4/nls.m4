dnl
dnl $Id$
dnl
dnl Part of edelib.
dnl Copyright (c) 2000-2007 EDE Authors.
dnl 
dnl This program is licenced under terms of the 
dnl GNU General Public Licence version 2 or newer.
dnl See COPYING for details.

dnl Set --enable-nls option, with checking of additional functions
AC_DEFUN([EDE_NLS], [
	AC_MSG_CHECKING(whether NLS is requested)
	AC_ARG_ENABLE(nls, [  --disable-nls           disable locale support],,enable_nls=yes)
	if eval "test $enable_nls = yes"; then
		AC_MSG_RESULT(yes)

		dnl _AS_ECHO(checking for needed NLS functions...)
		AC_CHECK_HEADER(locale.h, [have_locale_h=yes], [have_locale_h=no])
		AC_CHECK_FUNC(textdomain, [have_textdomain=yes], [have_textdomain=no])
		AC_CHECK_FUNC(bindtextdomain, [have_bindtextdomain=yes], [have_bindtextdomain=no])
		AC_CHECK_FUNC(setlocale, [have_setlocale=yes], [have_setlocale=no])

		if eval "test $have_locale_h = yes" && \
		   eval "test $have_textdomain = yes" && \
		   eval "test $have_bindtextdomain = yes" && \
		   eval "test $have_setlocale = yes"; then
				AC_DEFINE(USE_NLS, 1, [Define to 1 if you want Native Language Support])
		fi
	else
		AC_MSG_RESULT(no)
	fi
])
