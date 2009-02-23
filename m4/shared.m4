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

dnl Add --enable-shared option
dnl Also the bone of this code is based from FLTK's configure.in script
AC_DEFUN([EDELIB_SHARED], [
	AC_ARG_ENABLE(shared, [  --enable-shared         enable shared library],,enable_shared=no)
	if eval "test $enable_shared = no"; then
		dnl for jam
		SHARED=0
	else
		SHARED=1
	fi
])
