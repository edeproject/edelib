#!/bin/sh 

bin="../../tools/edelib-theme-script/edelib-theme-script"
ret=0

run_file() {
	tmp_file=".out"
	$bin $1 | tee -a $tmp_file

	grep FAILED $tmp_file 1> /dev/null
	[ "x$?" = "x0" ] && ret=1

	rm -f $tmp_file
}

run_file unittest.ss
run_file r5rs.ss
run_file edelib.ss

exit $ret
