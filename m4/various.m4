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

dnl --enable-debug and --enable-profile options
AC_DEFUN([EDELIB_DEVELOPMENT], [
	AC_ARG_ENABLE(debug, [  --enable-debug          enable debug],,enable_debug=no)
	if test "$enable_debug" = yes; then
		DEBUG_FLAGS="$DEBUG_FLAGS -g3"
		dnl clear all optimization flags
		OPTIM_FLAGS=""
	fi

	AC_ARG_ENABLE(profile, [  --enable-profile        enable profile],,enable_profile=no)
	if test "$enable_profile" = yes; then
		DEBUG_FLAGS="$DEBUG_FLAGS -pg"
		dnl clear all optimization flags
		OPTIM_FLAGS=""
	fi
])
