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

AC_DEFUN([EDELIB_MIME], [
	AC_MSG_CHECKING([for XDG MIME database])

	dnl check first if user already defined them
	xdg_data_home=$XDG_DATA_HOME
	xdg_data_dirs=$XDG_DATA_DIRS

	dnl if failed, use XDG default
	if test "x$xdg_data_home" = "x"; then
		xdg_data_home="$HOME/local/.share"
	fi

	if test "x$xdg_data_dirs" = "x"; then
		xdg_data_dirs="/usr/local/share:/usr/share:/usr/pkg/share"
	fi

	saved_ifs=$IFS
	
	dnl PATH_SEPARATOR is usualy ':' and is defined internaly by autoconf
	IFS=$PATH_SEPARATOR

	for splitted in "$xdg_data_home$PATH_SEPARATOR$xdg_data_dirs"; do
		for i in $splitted; do
			dnl just lookup for 'magic' file
			if test -f "$i/mime/magic"; then
				found_mime_path=$i
				break
			fi
		done
	done

	dnl restore it as soon as possible since AC_MSG_RESULT depends on it
	IFS="$saved_ifs"

	if test "x$found_mime_path" = "x"; then
		AC_MSG_RESULT(no)
	else
		AC_MSG_RESULT(yes)
	fi
])
