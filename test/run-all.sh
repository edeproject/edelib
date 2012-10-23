#!/bin/sh
# main driver for running all available tests
ret=0

echo "#######################"
echo "# edelib"
echo "#######################"

./run_tests
test "x$?" = "x1" && ret=1

echo ""
echo "#######################"
echo "# scheme"
echo "#######################"

cd scheme && ./run.sh
test "x$?" = "x1" && ret=1

# write something at the bottom
if test "x$ret" = "x0"; then
	echo "All tests passed."
else
	echo "Some of tests failed!!!"
fi

# if one of tests fails, mark all as failed
exit $ret
