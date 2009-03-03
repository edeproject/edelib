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

AC_DEFUN([EDELIB_LIBXPM], [
	AC_MSG_CHECKING([for libXpm])

	AC_LANG_SAVE
	AC_LANG_C
	AC_TRY_COMPILE([
		#include <X11/xpm.h>
	],[
		Pixmap pix, mask;
		Display* display;
		Drawable d;
		XpmCreatePixmapFromData(display, d, 0, &pix, &mask, 0);
	],[have_libxpm=yes],[have_libxpm=no])
	AC_LANG_RESTORE

	if test "$have_libxpm" = yes; then
		AC_DEFINE(HAVE_LIBXPM, 1, [Define to 1 if you have libXpm])
		AC_MSG_RESULT(yes)
		LIBXPM_LIBS="-lXpm"
	else
		AC_MSG_RESULT(no)
	fi
])
