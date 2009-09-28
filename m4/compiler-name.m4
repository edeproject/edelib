dnl
dnl $Id$
dnl
dnl Copyright (c) 2009 edelib authors
dnl Checker code was stolen from glib configure.in
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

dnl checks compiler name and fill 'ac_compiler_name' variable

AC_DEFUN([EDELIB_COMPILER_NAME], [
	AC_MSG_CHECKING([for compiler vendor])
	ac_compiler_name="unknown"

	AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#if defined(__SUNPRO_C) || (__SUNPRO_CC)
# else
# include "error: this is not SunStudio."
#endif
]], [[]])], [ ac_compiler_name="sun" ], [])

	AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#if defined(__INTEL_COMPILER) || (__ICC) || (__ECC)
# else
# include "error: this is not IntelCC."
#endif
]], [[]])], [ ac_compiler_name="intel" ], [])

	AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#if defined(__GNUC__)
# else
# include "error: this is not gcc."
#endif
]], [[]])], [ ac_compiler_name="gcc" ], [])

	AC_MSG_RESULT([$ac_compiler_name])
])
