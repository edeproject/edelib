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

AC_DEFUN([EDELIB_X11], [
	AC_PATH_XTRA

	dnl user does not have X installed
	dnl if we want let him build, he will explicitely must add '--without-x' option
	dnl since last one is for debugging purposes
	if test "$have_x" = no; then
		AC_MSG_ERROR(X11 libraries are not found! Please install them first)
	fi

	if test "$have_x" = disabled; then
		AC_MSG_NOTICE(*****************************************************)
		AC_MSG_NOTICE(* edelib will be built without X11 and FLTK support *)
		AC_MSG_NOTICE(*****************************************************)
	fi
])
