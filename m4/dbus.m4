dnl
dnl $Id$
dnl
dnl Part of edelib.
dnl Copyright (c) 2008 EDE Authors.
dnl 
dnl This program is licenced under terms of the 
dnl GNU General Public Licence version 2 or newer.
dnl See COPYING for details.

AC_DEFUN([EDELIB_CHECK_DBUS], [
	AC_ARG_ENABLE(dbus, [  --disable-dbus          disable D-BUS support],,[enable_dbus=yes])

	if eval "test $enable_dbus = yes"; then
		dnl long long does not exists in current C++ standard and
		dnl gcc with '-pedantic' will report it as error; this should get rid of it
		gcc_long_long_stuf="-Wno-long-long"

		EDELIB_PKG_CHECK_MODULES(DBUS, [dbus-1 >= 1.0], [have_dbus=yes], [have_dbus=no])

		if eval "test $pkg_config_found = yes"; then
			if eval "test $have_dbus = yes"; then
				AC_DEFINE(HAVE_DBUS, 1, [Define to 1 if you have libdbus])
				DBUS_CFLAGS="$gcc_long_long_stuf $DBUS_CFLAGS"
			else
				AC_MSG_RESULT(no)
				AC_MSG_ERROR([D-Bus libraries not found! Please install them first])
			fi
		fi

		dnl The hard way, pkg-config was not found so we must
		dnl scan manually for the library. 
		dnl
		dnl In case if pkg-config is used, it would append to macro's first
		dnl parameter _CFLAGS and _LIBS (e.g. DBUS_CFLAGS and DBUS_LIBS) so
		dnl I'm mimicking that here too.
		if eval "test $pkg_config_found = no"; then
			dbus_search_failed=no

			dbus_include_dirs="/usr/include /usr/include/dbus-1.0 /usr/local/include /usr/local/include/dbus-1.0"
			dbus_include_arch_deps="/usr/lib/dbus-1.0/include /usr/local/lib/dbus-1.0/include"
			dbus_lib_dirs="/lib /usr/lib /usr/local/lib"

			AC_MSG_CHECKING([for dbus/dbus.h])
			EDELIB_FIND_FILE(dbus/dbus.h, $dbus_include_dirs, dbus_incdir)

			if eval "test $dbus_incdir = no"; then
				dbus_search_failed=yes
				AC_MSG_RESULT(no)
			else
				AC_MSG_RESULT(yes)
			fi

			if eval "test $dbus_search_failed = no"; then
				AC_MSG_CHECKING([for dbus/dbus-arch-deps.h])
				EDELIB_FIND_FILE(dbus/dbus-arch-deps.h, $dbus_include_arch_deps, dbus_incdir_arch_deps)
				
				if eval "test $dbus_incdir_arch_deps = no"; then
					dbus_search_failed=yes
					AC_MSG_RESULT(no)
				else
					AC_MSG_RESULT(yes)
				fi
			fi

			if eval "test $dbus_search_failed = no"; then
				AC_MSG_CHECKING([for libdbus-1.so])
				EDELIB_FIND_FILE(libdbus-1.so, $dbus_lib_dirs, dbus_libdir)

				if eval "test $dbus_libdir = no"; then
					dbus_search_failed=yes
					AC_MSG_RESULT(no)
				else
					AC_MSG_RESULT(yes)
				fi
			fi

			if eval "test $dbus_search_failed = no"; then
				AC_DEFINE(HAVE_DBUS, 1, [Define to 1 if you have libdbus])

				DBUS_CFLAGS="$gcc_long_long_stuf -I$dbus_incdir -I$dbus_incdir_arch_deps"
				DBUS_LIBS="-L$dbus_libdir -ldbus-1"
			else
				AC_MSG_ERROR([D-Bus libraries not found! Please install them first])
			fi
		fi
	else
		DBUS_CFLAGS=""
		DBUS_LIBS=""
	fi
])
