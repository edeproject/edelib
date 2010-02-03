#!/bin/sh

# directories searched for translation files
target_dirs="edelib src"

# search patterns
patterns="*.h *.c *.cpp"

# template file
potfile="edelib.pot"

XGETTEXT="xgettext"
if [ "x$XGETTEXT" = "x" ]; then
	exit 0
fi

echo "Creating translation template..."

base=`pwd`
src=""
cd ..
for i in $patterns; do
	src="$src `find $target_dirs -name $i`"
done

$XGETTEXT -k'_' -k'N_' $src -o $base/$potfile
cd $base
