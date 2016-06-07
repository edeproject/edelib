#include <stdio.h>
#include <ctype.h>
#include <edelib/List.h>
#include <edelib/Functional.h>
#include "UnitTest.h"

EDELIB_NS_USE

static bool filter_char(char a) {
	return a == 'c';
}

static char to_upper(char c) {
	return toupper(c);
}

static int inc(int a) {
	return a + 5;
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

UT_FUNC(TestMap, "Test map()")
{
	list<char> c, U;
	const char* s = "test string";
	const char* s1 = "TEST STRING";
	for(int i = 0; i < 11; i++)
		c.push_back(s[i]);

	for(int i = 0; i < 11; i++)
		U.push_back(s1[i]);

	list<char> u;
	map(to_upper, c, u);

	UT_VERIFY( u == U );

	list<int> li, li2;
	list<int>::iterator li2_it, li_it;

	for(int i = 0; i < 20; i++)
		li.push_back(i);

	map(inc, li, li2);
	for(li_it = li.begin(), li2_it = li2.begin(); li2_it != li2.end(); ++li_it, ++li2_it)
		UT_VERIFY( (*li_it + 5) == *li2_it );
}

