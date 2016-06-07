dnl
dnl $Id$
dnl
dnl Stolen from KDE configure script. Original authors are below.
dnl Copyright (C) 1997 Janos Farkas (chexum@shadow.banki.hu)
dnl           (C) 1997,98,99 Stephan Kulow (coolo@kde.org)

dnl EDELIB_FIND_FILE(file, directories, directory_if_found_else_no)
AC_DEFUN([EDELIB_FIND_FILE], [
$3=no
for i in $2;
do
  for j in $1;
  do
    echo "configure: __oline__: $i/$j" >&AC_FD_CC
    if test -r "$i/$j"; then
      echo "taking that" >&AC_FD_CC
      $3=$i
      break 2
    fi
  done
done
])

