#
# $Id: Jamfile 3233 2012-04-03 11:48:19Z karijes $
#
# Copyright (c) 2005-2012 edelib authors
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library. If not, see <http://www.gnu.org/licenses/>.

AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS  = -I m4

INCLUDES = 	-I$(top_srcdir) \
			-DE_LOG_DOMAIN=\"edelib\"

bin_PROGRAMS = \
	tools/edelib-script/edelib-script                       \
	tools/edelib-dbus-introspect/edelib-dbus-introspect     \
	tools/edelib-update-font-cache/edelib-update-font-cache

dist_bin_SCRIPTS = \
	tools/edelib-convert-icontheme \
	tools/edelib-mk-indextheme

noinst_PROGRAMS = \
	test/run_tests

lib_LTLIBRARIES  = lib/libedelib.la     \
				   lib/libedelib_gui.la \
				   lib/libedelib_dbus.la

### librarires

xdgmime_files = \
	src/xdgmime/xdgmime.c \
	src/xdgmime/xdgmimealias.c \
	src/xdgmime/xdgmimecache.c \
	src/xdgmime/xdgmimeglob.c \
	src/xdgmime/xdgmimeicon.c \
	src/xdgmime/xdgmimeint.c \
	src/xdgmime/xdgmimemagic.c \
	src/xdgmime/xdgmimeparent.c

tinyscheme_files = \
	src/ts/scheme.c \
	src/ts/utf8.c

pcre_files = src/pcre/pcre.c

lib_libedelib_la_SOURCES = \
	$(pcre_files) \
	$(tinyscheme_files) \
	$(xdgmime_files) \
	src/Missing.c \
	src/Debug.c   \
	src/Scheme.cpp \
	src/Color.cpp  \
	src/ColorDb.cpp \
	src/Config.cpp  \
	src/DateTime.cpp \
	src/DesktopFile.cpp \
	src/Directory.cpp \
	src/File.cpp \
	src/FileTest.cpp \
	src/IconTheme.cpp \
	src/Listener.cpp \
	src/MimeType.cpp \
	src/Nls.cpp \
	src/Pty.cpp \
	src/PtyProcess.cpp \
	src/Regex.cpp \
	src/Resource.cpp \
	src/Run.cpp \
	src/Sipc.cpp \
	src/StrUtil.cpp \
	src/String.cpp \
	src/TempFile.cpp \
	src/TiXml.cpp \
	src/TiXmlParser.cpp \
	src/Util.cpp

libedelib_includedir = $(includedir)/edelib
libedelib_include_HEADERS = \
	edelib/ColorDb.h \
	edelib/Color.h \
	edelib/Config.h \
	edelib/DateTime.h \
	edelib/Debug.h \
	edelib/DesktopFile.h \
	edelib/Directory.h \
	edelib/edelib-config.h \
	edelib/edelib-global.h \
	edelib/File.h \
	edelib/FileTest.h \
	edelib/for-each-macro.h \
	edelib/Functional.h \
	edelib/IconTheme.h \
	edelib/Listener.h \
	edelib/List.h \
	edelib/MimeType.h \
	edelib/Missing.h \
	edelib/Namespace.h \
	edelib/Nls.h \
	edelib/Pty.h \
	edelib/PtyProcess.h \
	edelib/Regex.h \
	edelib/Resource.h \
	edelib/Run.h \
	edelib/Scheme.h \
	edelib/Sipc.h \
	edelib/String.h \
	edelib/StrUtil.h \
	edelib/TempFile.h \
	edelib/TiXml.h \
	edelib/Util.h \
	edelib/Version.h

libedelib_ts_includedir = $(includedir)/edelib/ts
libedelib_ts_include_HEADERS = \
	edelib/ts/opdefines.h \
	edelib/ts/scheme.h \
	edelib/ts/scheme-private.h

lib_libedelib_la_LDFLAGS = -version-info @EDELIB_LIBTOOL_VERSION_STR@

sdbm_files = \
	src/sdbm/sdbm.c \
	src/sdbm/sdbm_hash.c \
	src/sdbm/sdbm_pair.c

lib_libedelib_gui_la_SOURCES = \
	$(sdbm_files) \
	src/AnimateBox.cpp \
	src/DirWatch.cpp \
	src/Ede.cpp \
	src/ExpandableGroup.cpp \
	src/FontCache.cpp \
	src/FontChooser.cpp \
	src/ForeignCallback.cpp \
	src/IconChooser.cpp  \
	src/IconLoader.cpp \
	src/MenuBar.cpp   \
	src/MenuBase.cpp   \
	src/MenuButton.cpp   \
	src/MenuItem.cpp \
	src/Menu_add.cpp   \
	src/Menu_global.cpp \
	src/MessageBox.cpp \
	src/Netwm.cpp \
	src/SchemeEditor.cpp \
	src/SevenSeg.cpp \
	src/TableBase.cpp  \
	src/Theme.cpp \
	src/ThemeLoader.cpp \
	src/Window.cpp \
	src/WindowUtils.cpp \
	src/XSettingsClient.cpp \
	src/XSettingsCommon.cpp \
	src/XSettingsManager.cpp

libedelib_gui_includedir = $(includedir)/edelib
libedelib_gui_include_HEADERS = \
	edelib/AnimateBox.h \
	edelib/DirWatch.h \
	edelib/Ede.h \
	edelib/ExpandableGroup.h \
	edelib/FontCache.h \
	edelib/FontChooser.h \
	edelib/IconChooser.h \
	edelib/IconLoader.h \
	edelib/MenuBar.h \
	edelib/MenuBase.h \
	edelib/MenuButton.h \
	edelib/MenuItem.h \
	edelib/MessageBox.h \
	edelib/Netwm.h \
	edelib/SchemeEditor.h \
	edelib/SevenSeg.h \
	edelib/TableBase.h \
	edelib/Theme.h \
	edelib/ThemeLoader.h \
	edelib/Window.h \
	edelib/WindowUtils.h \
	edelib/WindowXid.h \
	edelib/XSettingsClient.h \
	edelib/XSettingsCommon.h \
	edelib/XSettingsManager.h

lib_libedelib_gui_la_LDFLAGS = -version-info @EDELIB_LIBTOOL_VERSION_STR@

lib_libedelib_dbus_la_SOURCES = \
	src/EdbusConnection.cpp \
	src/EdbusData.cpp \
	src/EdbusDict.cpp \
	src/EdbusError.cpp \
	src/EdbusList.cpp \
	src/EdbusMessage.cpp \
	src/EdbusObjectPath.cpp

libedelib_dbus_includedir = $(includedir)/edelib
libedelib_dbus_include_HEADERS = \
	edelib/EdbusConnection.h \
	edelib/EdbusContainer.h \
	edelib/EdbusData.h \
	edelib/EdbusDict.h \
	edelib/EdbusError.h \
	edelib/EdbusList.h \
	edelib/EdbusMessage.h \
	edelib/EdbusObjectPath.h

lib_libedelib_dbus_la_CFLAGS   = @DBUS_CFLAGS@
lib_libedelib_dbus_la_CXXFLAGS = @DBUS_CFLAGS@
lib_libedelib_dbus_la_LDFLAGS = -version-info @EDELIB_LIBTOOL_VERSION_STR@

### programs

# unit tests
test_run_tests_SOURCES = \
	test/UnitTest.cpp \
	test/utest.cpp \
	test/util.cpp \
	test/missing.cpp \
	test/strutil.cpp \
	test/file.cpp \
	test/file_test.cpp \
	test/directory.cpp \
	test/datetime.cpp \
	test/conf.cpp \
	test/resource.cpp \
	test/desktopfile.cpp \
	test/string.cpp \
	test/list.cpp \
	test/regex.cpp \
	test/color.cpp \
	test/colordb.cpp \
	test/xml.cpp \
	test/mime.cpp \
	test/temp_file.cpp \
	test/functional.cpp \
	test/run.cpp \
	test/run_tests.cpp \
	test/dbus.cpp  \
	test/xsettings.cpp \
	test/theme.cpp

test_run_tests_CXXFLAGS = @FLTK_CFLAGS@ @DBUS_CFLAGS@
test_run_tests_LDADD = \
	lib/libedelib_dbus.la \
	lib/libedelib_gui.la \
	lib/libedelib.la \
	@FLTK_LIBS_FULL@ @DBUS_LIBS@

tools_edelib_script_edelib_script_SOURCES = tools/edelib-script/edelib-script.cpp
tools_edelib_script_edelib_script_LDADD = lib/libedelib.la

tools_edelib_dbus_introspect_edelib_dbus_introspect_SOURCES  = tools/edelib-dbus-introspect/edelib-dbus-introspect.cpp
tools_edelib_dbus_introspect_edelib_dbus_introspect_CXXFLAGS = @FLTK_CFLAGS@ @DBUS_CFLAGS@
tools_edelib_dbus_introspect_edelib_dbus_introspect_LDADD = \
	lib/libedelib_dbus.la \
	lib/libedelib_gui.la \
	lib/libedelib.la \
	@FLTK_LIBS_FULL@ @DBUS_LIBS@

tools_edelib_update_font_cache_edelib_update_font_cache_SOURCES = tools/edelib-update-font-cache/edelib-update-font-cache.cpp
tools_edelib_update_font_cache_edelib_update_font_cache_CXXFLAGS = @FLTK_CFLAGS@
tools_edelib_update_font_cache_edelib_update_font_cache_LDADD = \
	lib/libedelib_gui.la \
	lib/libedelib.la \
	@FLTK_LIBS_FULL@

### pkgcondif stuff

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = edelib.pc edelib-gui.pc edelib-gui-no-images.pc edelib-dbus.pc

### documentation

dist_doc_DATA = README COPYING 

install-data-hook:
	cd doc && cp -R html $(docdir)

### scheme files

sslibdir = $(libdir)/edelib/sslib
sslib_DATA = \
	sslib/init.ss \
	sslib/init-2.ss \
	sslib/theme.ss

### distclean

DISTCLEANFILES = \
	edelib.pc \
	edelib-gui.pc \
	edelib-gui-no-images.pc \
	edelib-dbus.pc \
	config.guess \
	config.h \
	config.sub \
	configure \
	depcomp \
	install-sh \
	ltmain.sh \
	missing \
	Makefile.in \
	aclocal.m4 \
	edelib/_conf.h.in \
	edelib/edelib-config.h

clean-local:
	rm -Rf doc/html

html:
if HAVE_DOXYGEN
	@echo "Building documentation..."
	@cd doc && doxygen
else
	@echo "*** doxygen not found, documentation will not be built."
endif

check-local:
	cd test && ./run-all.sh

uninstall-local:
	rm -Rf $(docdir)/html

distclean-local:
	rm -Rf autom4te.cache

dist: html
