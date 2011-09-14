#include <edelib/List.h>
#include <edelib/Functional.h>
#include "UnitTest.h"
#include <stdio.h>

EDELIB_NS_USE

bool filter_char(char a) {
	return a == 'c';
}

UT_FUNC(TestFilter, "Test filter()")
{
	list<char> lc, lcf;
	lc.push_back('a');
	lc.push_back('c');
	lc.push_back('b');
	lc.push_back('b');
	lc.push_back('c');
	lc.push_back('a');

	UT_VERIFY( filter(filter_char, lc, lcf) == 2 );

	list<char>::iterator cit = lcf.begin();
	for(; cit != lcf.end(); ++cit)
		UT_VERIFY( *cit == 'c' );

	lc.clear();
	lcf.clear();
	UT_VERIFY( filter(filter_char, lc, lcf) == 0 );

	lc.push_back('a');
	lc.push_back('b');
	lc.push_back('d');
	UT_VERIFY( filter(filter_char, lc, lcf) == 0 );
}
