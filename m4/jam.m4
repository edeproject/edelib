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

dnl Check do we have jam installed and try to determine version
dnl where is minimal supported 2.3
AC_DEFUN([EDELIB_PROG_JAM], [
	AC_PATH_PROG(JAM, jam)

	if test -n "$JAM"; then
		AC_MSG_CHECKING([for jam version])

		echo "Echo \$(JAMVERSION) ;" > conftest.jam
		jam_version_orig=`$JAM -f conftest.jam | head -1`
		jam_version=`echo $jam_version_orig | sed -e 's/\.//g'`
		rm -f conftest.jam
		if test "$jam_version" -ge 23; then
			msg="$jam_version_orig (ok)"
			AC_MSG_RESULT($msg)
		else
			msg="jam version $jam_version_orig is too old. Download a newer version from our repository"
			AC_MSG_ERROR($msg)
		fi
	else
		AC_MSG_ERROR(Jam is missing! You can download it from our repository)
	fi
])
