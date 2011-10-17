#!/bin/sh
# main driver for running all available tests
ret=0

echo "#######################"
echo "# edelib"
echo "#######################"

./run_tests
[ "x$?" == "x1" ] && ret=1

echo ""
echo "#######################"
echo "# scheme"
echo "#######################"

cd scheme && ./run.sh
[ "x$?" == "x1" ] && ret=1

# if one of tests fails, mark all as failed
exit $ret
