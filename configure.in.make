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

m4_define([edelib_major_version], [2])
m4_define([edelib_minor_version], [1])
m4_define([edelib_patch_version], [0])
m4_define([edelib_version], [edelib_major_version.edelib_minor_version.edelib_patch_version])

AC_PREREQ(2.13)

AC_INIT(edelib, edelib_version, [karijes@users.sourceforge.net])
# ---
AM_INIT_AUTOMAKE([AC_PACKAGE_TARNAME], [AC_PACKAGE_VERSION])
# ---
AM_SILENT_RULES([yes])

AC_CONFIG_SRCDIR(edelib/Nls.h)
AC_CONFIG_HEADER(edelib/_conf.h:edelib/_conf.h.in)
dnl AC_CONFIG_MACRO_DIR([m4])

EDELIB_MAJOR_VERSION=edelib_major_version
EDELIB_MINOR_VERSION=edelib_minor_version
EDELIB_PATCH_VERSION=edelib_patch_version
EDELIB_API_VERSION=$PACKAGE_VERSION

dnl for libtool versioning
EDELIB_LIBTOOL_VERSION_STR="$EDELIB_MAJOR_VERSION:$EDELIB_MINOR_VERSION:$EDELIB_PATCH_VERSION"

dnl configure fuck up CFLAGS/CXXFLAG by adding '-O2' and '-g' after AC_PROG_CC and AC_PROG_CXX
dnl and there is no way to remove them without knowing if user added them
user_cflags="$CFLAGS"
user_cxxflags="$CXXFLAGS"

dnl check for compiler and programs first before any variable was set
dnl this is for case if compiler is not a gcc, so we can set a different variables
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LN_S
AC_PATH_PROG(SED, sed)
AC_PATH_PROG(MSGFMT, msgfmt)
AC_PATH_PROG(XGETTEXT, xgettext)
AC_PATH_PROG(DOXYGEN, doxygen)
AM_CONDITIONAL(HAVE_DOXYGEN, test "x$DOXYGEN" != "x")

AC_PROG_LIBTOOL

EDELIB_COMPILER_VENDOR
case $ac_compiler_vendor in 
	"gcc")
	SANITY_FLAGS="-Wall -pedantic -Wshadow"
	DEBUG_FLAGS="-D_DEBUG"
	OPTIM_FLAGS="-O2"
	;;
	"sun")
	SANITY_FLAGS="+w2"
	DEBUG_FLAGS="-D_DEBUG"
	OPTIM_FLAGS="-xO3"
	;;
	*)
	SANITY_FLAGS=""
	DEBUG_FLAGS="-D_DEBUG"
	OPTIM_FLAGS=""
	;;
esac

CFLAGS="-DHAVE_EDELIB_BASE_CONFIG_H $SANITY_FLAGS $user_cflags"
CXXFLAGS="-DHAVE_EDELIB_BASE_CONFIG_H $SANITY_FLAGS $user_cxxflags"

dnl basic headers
AC_HEADER_STDC
AC_CHECK_HEADER(string.h, AC_DEFINE(HAVE_STRING_H, 1, [Define to 1 if you have string.h file]))
AC_CHECK_FUNC(snprintf, AC_DEFINE(HAVE_SNPRINTF, 1, [Define to 1 if you have snprintf()]))
AC_CHECK_FUNC(vsnprintf, AC_DEFINE(HAVE_VSNPRINTF, 1, [Define to 1 if you have vsnprintf()]))

dnl freebsd have different unsetenv() impl. and is not compatible with our implementation
AC_CHECK_FUNC(unsetenv, AC_DEFINE(HAVE_UNSETENV, 1, [Define to 1 if you ave unsetenv()]))

dnl terminal stuff
AC_CHECK_FUNC(grantpt, AC_DEFINE(HAVE_GRANTPT, 1, [Define to 1 if you have grantpt()]))
AC_CHECK_FUNC(ptsname, AC_DEFINE(HAVE_PTSNAME, 1, [Define to 1 if you have ptsname()]))
AC_CHECK_FUNC(unlockpt, AC_DEFINE(HAVE_UNLOCKPT, 1, [Define to 1 if you have unlockpt()]))
AC_CHECK_FUNC(_getpty, AC_DEFINE(HAVE__GETPTY, 1, [Define to 1 if you have _getpty()]))
AC_CHECK_HEADER(pty.h, AC_DEFINE(HAVE_PTY_H, 1, [Define to 1 if you have pty.h]))
AC_CHECK_HEADER(libutil.h, AC_DEFINE(HAVE_LIBUTIL_H, 1, [Define to 1 if you have libutil.h]))
AC_CHECK_HEADER(util.h, AC_DEFINE(HAVE_UTIL_H, 1, [Define to 1 if you have util.h]))

dnl xdgmimcache.c
AC_CHECK_FUNC(mmap, AC_DEFINE(HAVE_MMAP, 1, [Define to 1 if you have mmap()]))

EDELIB_CPP_VARARGS
EDELIB_DATETIME
EDELIB_DEVELOPMENT

EDELIB_X11

dnl user fired --without-x, we disable FLTK checks
if test "$have_x" = "yes"; then
	EDELIB_FLTK
	EDELIB_LIBXPM
fi

EDELIB_NOTIFY
EDELIB_SHARED
EDELIB_NLS
EDELIB_MIME
EDELIB_DBUS
EDELIB_LARGEFILE
EDELIB_DMALLOC

AC_ARG_VAR([PKG_CONFIG], [pkg-config command])
AC_ARG_VAR([PKG_CONFIG_PATH], [A directory with .pc files for pkg-config])

dnl jam don't like spaces in empty string so clear vars if --without-x is given
if test "$have_x" = "yes"; then
	FLTK_LIBS="$FLTK_LIBS $LIBXPM_LIBS $LIBFAM_LIBS"
	FLTK_LIBS_FULL="$FLTK_LIBS_FULL $LIBXPM_LIBS $LIBFAM_LIBS"
else
	FLTK_LIBS=""
	FLTK_LIBS_FULL=""
fi

if test -n "$OPTIM_FLAGS"; then
	CFLAGS="$CFLAGS $OPTIM_FLAGS"
	CXXFLAGS="$CXXFLAGS $OPTIM_FLAGS"
else
	CFLAGS="$CFLAGS $DEBUG_FLAGS"
	CXXFLAGS="$CXXFLAGS $DEBUG_FLAGS"
fi

CFLAGS="$CFLAGS $LARGEFILE"
CXXFLAGS="$CXXFLAGS $LARGEFILE"

dnl this is issued from buildbot so edelib can be used from local directory
if test -n "$EDELIB_USE_LOCALDIR"; then
	prefix=`pwd`
	ln -s . include
fi

EDELIB_INIT_JAM

AC_SUBST(EDELIB_MAJOR_VERSION)
AC_SUBST(EDELIB_MINOR_VERSION)
AC_SUBST(EDELIB_PATCH_VERSION)
AC_SUBST(EDELIB_API_VERSION)
AC_SUBST(EDELIB_LIBTOOL_VERSION_STR)
AC_SUBST(FLTK_CFLAGS)
AC_SUBST(FLTK_LIBS)
AC_SUBST(FLTK_LIBS_FULL)
AC_SUBST(DBUS_CFLAGS)
AC_SUBST(DBUS_LIBS)
AC_SUBST(SHARED)
AC_SUBST(CFLAGS)
AC_SUBST(CXXFLAGS)
AC_SUBST(LDFLAGS)
AC_SUBST(LDLIBS)
AC_SUBST(XDG_MIME_HAVE_MMAP)
AC_SUBST(SCHEME_FLAGS)
AC_SUBST(DMALLOC_LIBC)
AC_SUBST(DMALLOC_LIBCXX)

dnl specific stuf which goes to edelib-config.h
AC_CONFIG_COMMANDS([outfile], [
 cat > $outfile <<\_____EOF
/* generated file; please modify configure.in */
#ifndef __EDELIB_EDELIB_CONFIG_H__
#define __EDELIB_EDELIB_CONFIG_H__

_____EOF

 if test "x$have_iso_varargs" = "xyes"; then
	 echo "#define EDELIB_HAVE_ISO_VARARGS 1" >> $outfile
 fi

 if test "x$have_gnuc_varargs" = "xyes"; then
	 echo "#define EDELIB_HAVE_GNUC_VARARGS 1" >> $outfile
 fi

 if test "x$have_dbus" = "xyes"; then
	 echo "#define EDELIB_HAVE_DBUS 1" >> $outfile
 fi

 if test "x$have_libxpm" = "xyes"; then
	 echo "#define EDELIB_HAVE_LIBXPM 1" >> $outfile
 fi

 if test "x$fltk_have_xft" != "x"; then
	 echo "#define EDELIB_HAVE_FLTK_XFT 1" >> $outfile
 fi

 if test "x$prefix" != "x"; then
	 echo "#define EDELIB_INSTALL_PREFIX \"$prefix\"" >> $outfile
 fi

 cat >> $outfile <<\_____EOF

#endif
_____EOF
], [
 outfile="edelib/edelib-config.h"
 have_iso_varargs=$have_iso_varargs
 have_gnuc_varargs=$have_gnuc_varargs
 have_dbus=$have_dbus
 have_libxpm=$have_libxpm
 fltk_have_xft=$fltk_have_xft
 prefix=$prefix
])

AC_OUTPUT([
 Makefile
 edelib/Version.h
 doc/Doxyfile
 edelib.pc
 edelib-gui.pc
 edelib-gui-no-images.pc
 edelib-dbus.pc
 po/update-messages.sh
])

chmod +x po/update-messages.sh

# generate scheme include files
cd sslib
./gen-c-string.sh init.ss  > init_ss.h
./gen-c-string.sh theme.ss > theme_ss.h
cd ..

# create config.h
echo "#include \"edelib/_conf.h\"" > config.h
