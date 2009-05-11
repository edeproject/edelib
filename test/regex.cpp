#include <edelib/Regex.h>
#include "UnitTest.h"
#include <stdio.h>

EDELIB_NS_USE

UT_FUNC(RegexMatchTest, "Test regex match")
{
	Regex rx;
	UT_VERIFY( rx == false );

	rx.compile("[a-zA-Z]+");
	UT_VERIFY( rx == true );
	UT_VERIFY( rx.match("foo") );

	rx.compile("[A-Z]+");
	UT_VERIFY( rx.match("foo") );

	Regex::MatchVec m;
	UT_VERIFY( rx.match("fooFOo", 0, &m) );

	Regex::MatchVec::iterator it = m.begin();
	UT_VERIFY( (*it).offset == 3 );
	UT_VERIFY( (*it).length == 2 );

	rx.compile("[A-Z]+", RX_CASELESS | RX_EXTENDED);
	UT_VERIFY( rx.match("foo") );
	UT_VERIFY( rx.match("abAbcdCDDDDABDCddc") );
	UT_VERIFY( rx.match("abAbcdCDD DDABDCddc") );
	UT_VERIFY( rx.match("abAbcdCDD DDABDCddc") );
	UT_VERIFY( rx.match("abAbcdCDD12345") );
	UT_VERIFY( rx.match("12345") );

	rx.compile("^foo", RX_MULTILINE);
	UT_VERIFY( rx == true );
	UT_VERIFY( rx.match("\nfoo") );

	rx.compile("^foo", RX_EXTENDED);
	UT_VERIFY( rx == true );
	UT_VERIFY( rx.match("\nfoo") );

	UT_VERIFY( rx.compile("(\\s+sample1$)|( or-sample2 )") == true );
	UT_VERIFY( rx.match("this is sample1") );
	UT_VERIFY( rx.match("Blah or-sample2 blah blah blah") );
	UT_VERIFY( rx.match("or-sample2 is at start and will fail") != 1 );
}

UT_FUNC(RegexSplitTest, "Test regex split")
{
	Regex rx;
	rx.compile("[ \t]+");
	list<String> ls;
	UT_VERIFY( rx.split("this is 1234 sample", ls) == 4 );

	list<String>::iterator it = ls.begin();

	UT_VERIFY( (*it) == "this" ); ++it;
	UT_VERIFY( (*it) == "is" ); ++it;
	UT_VERIFY( (*it) == "1234" ); ++it;
	UT_VERIFY( (*it) == "sample" );

	// perl/python compatibility
	ls.clear();
	rx.compile("[^A-Z]+");
	UT_VERIFY( rx.split("this is anoTher sample 345 $$$ Or bla", ls) == 4 );
	it = ls.begin();
	UT_VERIFY( (*it) == "" ); ++it;
	UT_VERIFY( (*it) == "T" ); ++it;
	UT_VERIFY( (*it) == "O" ); ++it;
	UT_VERIFY( (*it) == "" );
}

UT_FUNC(RegexSplitTest2, "Test regex wrinkle split")
{
	Regex rx;
	list<String> ls;
	list<String>::iterator it;

	/*
	 * if split() internaly is not checked, this will go into infinite loop
	 * when only characters are given in split()
	 */
	rx.compile("[a-z]*");
	UT_VERIFY( rx.split("some random text without meaning", ls) == 2 );
	ls.clear();
}
