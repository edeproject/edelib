#
# $Id$
#
# Part of edelib.
# Copyright (c) 2005-2006 EDE Authors.
#
# This program is licenced under terms of the 
# GNU General Public Licence version 2 or newer.
# See COPYING for details.

# Tests running driver.
# Used to run all executable files in this directory
# or those that are passed as option.

SCRIPT="run.sh"
ERRTMP=".output.stderr"
OUTTMP=".output.stdout"

help()
{
	echo "Usage: $0 [OPTIONS]"
	echo "Runs files in this directory as part of testsuite."
	echo "Options:"
	echo "   -e, --exec [file]  run only [file]"
	echo "   -h, --help         you see it"
	echo ""
	exit 1
}

print_once()
{
	if [ "$pod" = "" ]; then
		echo "$1"
		pod="1"
	fi
}

print_logs()
{
	errsz=`du -k "$ERRTMP" | awk '{print $1}'`
	outsz=`du -k "$OUTTMP" | awk '{print $1}'`

	if [ "$errsz" != "0" ]; then
		echo ""
		echo "Errors"
		echo "=================================="
		cat "$ERRTMP"
	fi

	if [ "$outsz" != "0" ]; then
		echo ""
		echo "Output"
		echo "=================================="
		cat "$OUTTMP"
	fi
}

clean_junk()
{
	if [ -f "$OUTTMP" ]; then
		rm "$OUTTMP"
	fi

	if [ -f "$ERRTMP" ]; then
		rm "$ERRTMP"
	fi
}

exec_file()
{
	target=$1
	if [ ! -f "$target" ]; then
		echo "'$target' does not exists"
		exit 1
	fi

	if [ ! -x "$target" ]; then
		echo "'$target' is not executable"
		exit 1
	fi

	# means program failed
	eflag=""
	# means program passed
	pflag=""

	# run it
	`"$target" >> $OUTTMP 2>> $ERRTMP`

	if [ "$?" = "0" ]; then
		pflag="x"
	else
		eflag="x"
	fi

	print_once "Test name       | Passed | Failed"
	printf "%-16.16s    %-9s%-9s\n" "$target" "$pflag" "$eflag"
}

exec_all()
{
	all=`ls -1`
	for f in $all; do
		if [ -x "$f" ] && [ "$f" != "$SCRIPT" ]; then
			exec_file "$f"
		fi
	done
}


#
# Main part
#

echo ""

# clean junk generated before
clean_junk

# no options
if [ $# -eq 0 ]; then
	exec_all
fi

# with some option(s)
for argv in "$@"; do
	case $argv in 
		-h | --help)
		help
		continue;;

		-e | --exec)
		shift
		if [ -z "$1" ]; then
			echo "Missing parameter"
			exit 1
		fi
		exec_file $1
		continue;;

		-*)
		echo "Unknown option '$argv'"
		echo "Run $SCRIPT -h to see them"
		exit 1
	esac
done

# print anything logged, if did
if [ -f "$OUTTMP" ] && [ -f "$ERRTMP" ]; then
	print_logs
fi

