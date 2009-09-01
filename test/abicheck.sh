#!/bin/sh
# Shamelessly stolen from glib and modified to be used with C++.
#
# Before anything can be done, first symbols must be generated, and that was done with:
# nm -g --defined-only targetlib | cut -d ' ' -f 3 | c++filt | sed -e '/^$/d' | sort > targetlib.symbols

function check {
	cat $1 | sed -e '/^$/d' | sort > expected-abi
	#nm -D -g --defined-only "../lib/$2" | cut -d ' ' -f 3 | c++filt | sort > actual-abi
	nm -g --defined-only "../lib/$2" | cut -d ' ' -f 3 | c++filt | sed -e '/^$/d' | sort > actual-abi
	diff -u expected-abi actual-abi && rm -f expected-abi actual-abi
}

# to actual checking
check edelib.symbols      libedelib.a
check edelib_gui.symbols  libedelib_gui.a
check edelib_dbus.symbols libedelib_dbus.a
