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

dnl Set --enable-nls option, with checking of additional functions
AC_DEFUN([EDELIB_NLS], [
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
				dnl flags for tinyscheme
				SCHEME_FLAGS="-DUSE_GETTEXT"
		fi
	else
		AC_MSG_RESULT(no)
	fi
])
