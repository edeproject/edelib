#include <edelib/Regex.h>
#include "UnitTest.h"
#include <stdio.h>

using namespace edelib;

UT_FUNC(RegexMatchTest, "Test regex match")
{
	Regex rx;
	UT_VERIFY( rx == false );

	rx.compile("[a-zA-Z]+");
	UT_VERIFY( rx.match("foo") );
	UT_VERIFY( rx == true );

	rx.compile("[A-Z]+");
	UT_VERIFY( rx.match("foo") == false );

	rx.compile("[A-Z]+", RX_ICASE | RX_EXTENDED);
	UT_VERIFY( rx.match("foo") );
	UT_VERIFY( rx.match("abAbcdCDDDDABDCddc") );
	UT_VERIFY( rx.match("abAbcdCDD DDABDCddc") );
	UT_VERIFY( rx.match("abAbcdCDD DDABDCddc") );
	UT_VERIFY( rx.match("abAbcdCDD12345") );
	UT_VERIFY( rx.match("12345") == false );

	rx.compile("^foo", RX_EXTENDED | RX_NEWLINE);
	UT_VERIFY( rx == true );
	UT_VERIFY( rx.match("\nfoo") );

	rx.compile("^foo", RX_EXTENDED);
	UT_VERIFY( rx == true );
	UT_VERIFY( rx.match("\nfoo") == false );

	UT_VERIFY( rx.compile("(\\s+sample1$)|( or-sample2 )") == true );
	UT_VERIFY( rx.match("this is sample1") );
	UT_VERIFY( rx.match("Blah or-sample2 blah blah blah") );
	UT_VERIFY( rx.match("or-sample2 is at start and will fail") == false );
}

UT_FUNC(RegexSearchTest, "Test regex search")
{
	Regex rx;
	rx.compile("[a-z]+");
	int p = 0;
	int r = rx.search("123456 34567 this is demo", p);
	UT_VERIFY( r != -1 );
	UT_VERIFY( r == 13 );
	UT_VERIFY( p == 4 );

	p = 0;
	r = rx.search("12345678", p);
	UT_VERIFY( p == 0 );
	UT_VERIFY( r == -1 );

	rx.compile("[a-z]*");
	p = 0;
	// FIXME: is this okay or p should be length of searched string ???
	r = rx.search("123456 34567 this is demo", p);
	UT_VERIFY( p == 0 );
	UT_VERIFY( r == 0 );
}

UT_FUNC(RegexSplitTest, "Test regex split")
{
	Regex rx;
	rx.compile("[^ \t]+");
	list<String> ls;
	UT_VERIFY( rx.split("this is 1234 sample", ls) == 4 );

	list<String>::iterator it = ls.begin();
	UT_VERIFY( (*it) == "this" ); ++it;
	UT_VERIFY( (*it) == "is" ); ++it;
	UT_VERIFY( (*it) == "1234" ); ++it;
	UT_VERIFY( (*it) == "sample" );

	ls.clear();
	rx.compile("[A-Z]+");
	UT_VERIFY( rx.split("this is anoTher sample 345 $$$ Or bla", ls) == 2 );
	it = ls.begin();
	UT_VERIFY( (*it) == "T" ); ++it;
	UT_VERIFY( (*it) == "O" );

	ls.clear();
	rx.compile("<\\s*A\\s+[^>]*href\\s*=\\s*\"([^\"]*)\"", RX_EXTENDED | RX_ICASE);
	UT_VERIFY( rx.split("<b>this is</b> <a href=\"www.bla.com\">bla</a> url. Another <A HREF=\"x.com\">one</A>.", ls) == 2 );
	it = ls.begin();
	UT_VERIFY( (*it) == "<a href=\"www.bla.com\"" ); ++it;
	UT_VERIFY( (*it) == "<A HREF=\"x.com\"" );
}


UT_FUNC(RegexSplitTest2, "Test regex wrinkle split")
{
	Regex rx;
	/*
	 * if split() internaly is not checked, this will go into infinite loop
	 * when only characters are given in split()
	 */
	rx.compile("[a-z]*");
	list<String> ls;
	list<String>::iterator it;

	// FIXME: bug or ??? Maybe should return 1 only
	UT_VERIFY( rx.split("only first word must be given", ls) == 2 );
	it = ls.begin();
	UT_VERIFY( (*it) == "only" ); ++it;
	UT_VERIFY( (*it) == "" );
}
