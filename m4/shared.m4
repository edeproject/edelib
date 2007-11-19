dnl
dnl $Id$
dnl
dnl Part of edelib.
dnl Copyright (c) 2000-2007 EDE Authors.
dnl 
dnl This program is licenced under terms of the 
dnl GNU General Public Licence version 2 or newer.
dnl See COPYING for details.

dnl Add --enable-shared option
dnl Also the bone of this code is based from FLTK's configure.in script
AC_DEFUN([EDELIB_SHARED], [
	AC_ARG_ENABLE(shared, [  --enable-shared         enable shared library],,enable_shared=no)
	if eval "test $enable_shared = no"; then
		SHAREDCXXFLAGS=""
		SHAREDLDFLAGS=""
	else
		dnl for jam
		SHARED=1
		ver_suffix="$EDELIB_MAJOR_VERSION.$EDELIB_MINOR_VERSION.$EDELIB_PATCH_VERSION"
		uname=`uname`
		case $uname in
			Linux* | *BSD*)
				DSOFLAGS="-shared -fPIC"
				DSOLDFLAGS="-Wl,-rpath,$libdir"
				DSOSUFFIX=".so.$ver_suffix"
				DSOSYMLINK=".so"
			;;
			*)
				AC_MSG_WARN(Shared libraries my not be supported. Disabling for now.)
				SHARED=0
			;;
		esac
	fi
])
