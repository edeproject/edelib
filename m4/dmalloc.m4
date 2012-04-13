dnl
dnl $Id$
dnl
dnl Copyright (c) 2005-2012 edelib authors
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

AC_DEFUN([EDELIB_DMALLOC], [
	AC_ARG_WITH(dmalloc_path, [  --with-dmalloc-path=[DIR]    link edelib agains dmalloc])

	if test "x$with_dmalloc_path" != "x"; then
		DMALLOC_LIBC="-L$with_dmalloc_path/lib -ldmalloc"
		DMALLOC_LIBCXX="-L$with_dmalloc_path/lib -ldmallocxx"

		AC_MSG_NOTICE(using dmalloc from $with_dmalloc_path/lib)
	fi
])
