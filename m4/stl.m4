dnl
dnl $Id: various.m4 1856 2007-03-17 11:28:25Z karijes $
dnl
dnl Part of edelib.
dnl Copyright (c) 2000-2007 EDE Authors.
dnl 
dnl This program is licenced under terms of the 
dnl GNU General Public Licence version 2 or newer.
dnl See COPYING for details.

dnl Some distributions split packages between STL runtime
dnl and development versions; this will ensure we have development packages
dnl Code is based on AC_CXX_HAVE_STL from Todd Veldhuizen and Luc Maisonobe
AC_DEFUN([EDE_CHECK_STL], [
	AC_MSG_CHECKING(for reasonable STL support)
	AC_LANG_CPLUSPLUS
	AC_TRY_COMPILE([
		#include <list>
		#include <deque>
		using namespace std;
	],[
		list<int> x; x.push_back(5);
		list<int>::iterator iter = x.begin(); if (iter != x.end()) ++iter; return 0;
	], ac_cv_cxx_have_stl=yes, ac_cv_cxx_have_stl=no)

	if eval "test $ac_cv_cxx_have_stl = no"; then
		AC_MSG_ERROR(You don't have STL (C++ standard library) packages installed, or your version of compiler is too old. Please fix this first)
	else
		AC_MSG_RESULT(yes)
	fi
])
