dnl
dnl $Id$
dnl
dnl Part of edelib.
dnl Copyright (c) 2000-2007 EDE Authors.
dnl 
dnl This program is licenced under terms of the 
dnl GNU General Public Licence version 2 or newer.
dnl See COPYING for details.

AC_DEFUN([EDELIB_MIME], [
	AC_MSG_CHECKING([for XDG MIME database])

	dnl check first if user already defined them
	xdg_data_home=$XDG_DATA_HOME
	xdg_data_dirs=$XDG_DATA_DIRS

	dnl if failed, use XDG default
	if test ! $xdg_data_home; then
		xdg_data_home="$HOME/local/.share"
	fi

	if test ! $xdg_data_dirs; then
		xdg_data_dirs="/usr/local/share:/usr/share"
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

	if test ! $found_mime_path; then
		AC_MSG_RESULT(no)
		dnl watch to quote ',' character inside AC_MSG_NOTICE or message will not be displayed
		AC_MSG_NOTICE(********************************************************************************************)
		AC_MSG_NOTICE(* Looks like you are missing MIME database. Please install appropriate package for your OS *)
		AC_MSG_NOTICE(* (usually named as shared-mime-info) or download archive from http://freedesktop.org.     *)
		AC_MSG_NOTICE(* edelib can be used without MIME database[,] but MIME types will not be recognized.         *)
		AC_MSG_NOTICE(********************************************************************************************)
	else
		AC_MSG_RESULT(yes)
	fi

	IFS="$saved_ifs"
])
