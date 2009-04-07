#!/bin/sh
# meant to be used from buildbot...

cd "test"
./run_tests
ret=$?
cd ..

# propagate return value from run_tests so environment can catch it
exit $ret
