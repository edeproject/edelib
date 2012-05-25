#!/bin/sh

aclocal -I m4 \
	  && libtoolize --quiet --force --copy \
	  && autoheader \
	  && automake --add-missing --foreign --copy \
	  && autoconf configure.in.make

if [ "$1" = "--compile" ]; then
	./configure --enable-debug --disable-shared --prefix=/tmp/ede && make
fi
