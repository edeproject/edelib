#include <edelib/String.h>
#include <stdlib.h>
#include "UnitTest.h"
#include <stdio.h>

using namespace edelib;

UT_FUNC(StringBasicTest, "Test basic string functions")
{
	String dstr;
	UT_VERIFY( dstr.length() == 0 );

	dstr.reserve(20);
	UT_VERIFY( dstr.capacity() == 20 );
	UT_VERIFY( dstr.length() == 0 );

	dstr = "Test string";
	UT_VERIFY( dstr.length() == 11 );

	dstr = "Test";
	UT_VERIFY( dstr.length() == 4 );

	dstr.clear();
	UT_VERIFY( dstr.length() == 0 );

	dstr.assign("test");
	UT_VERIFY( dstr.length() == 4 );
	dstr.append(" sample");
	UT_VERIFY( dstr.length() == 11 );
	dstr.append(" sample1");
	dstr.append(" sample2");
	dstr.append("sample3");
	UT_VERIFY( dstr.length() == 34 );
	UT_VERIFY( dstr == "test sample sample1 sample2sample3" );

	dstr.clear();
	dstr.append("Sample string", 4);
	UT_VERIFY( dstr == "Samp" );
	UT_VERIFY( dstr[0] == 'S' );
	UT_VERIFY( dstr[1] == 'a' );
	UT_VERIFY( dstr[2] == 'm' );
	UT_VERIFY( dstr[3] == 'p' );

	String dstr2(dstr);
	UT_VERIFY( dstr2 == dstr );

	dstr.clear();
	dstr2.clear();
	dstr = "Sample";
	dstr2.assign(dstr);
	UT_VERIFY( dstr2 == dstr );
	UT_VERIFY( dstr2 == "Sample" );

	dstr2.append(dstr);
	UT_VERIFY( dstr2 == "SampleSample" );
	dstr2.append(dstr2);
	UT_VERIFY( dstr2 == "SampleSampleSampleSample" );

	dstr.clear();
	dstr.append(10, 'F');
	UT_VERIFY( dstr == "FFFFFFFFFF" );
	dstr.append(10, 'B');
	UT_VERIFY( dstr == "FFFFFFFFFFBBBBBBBBBB" );

	dstr += "boo";
	UT_VERIFY( dstr == "FFFFFFFFFFBBBBBBBBBBboo" );

	dstr.append(1, '/');
	UT_VERIFY( dstr == "FFFFFFFFFFBBBBBBBBBBboo/" );
	dstr.append(5, 'c');
	UT_VERIFY( dstr == "FFFFFFFFFFBBBBBBBBBBboo/ccccc" );

	dstr += 'x';
	dstr += 'x';
	dstr += 'x';
	UT_VERIFY( dstr == "FFFFFFFFFFBBBBBBBBBBboo/cccccxxx" );

	dstr.clear();
	dstr += 'x';
	dstr += 'y';
	dstr += 'z';
	UT_VERIFY( dstr == "xyz" );

	dstr.clear();
	dstr.reserve(3);
	dstr = "abc";
	dstr += 'd';
	UT_VERIFY( dstr == "abcd" );
}

UT_FUNC(StringOperators, "Test string operators")
{
	String test("sample");
	UT_VERIFY( test != "Sample" );
	UT_VERIFY( test == "sample" );
	UT_VERIFY( test <= "sample" );
	UT_VERIFY( test >= "sample" );
	UT_VERIFY( test > "sa" );
	UT_VERIFY( test < "samplebla" );

	UT_VERIFY( "Sample" != test );
	UT_VERIFY( "sample" == test );
	UT_VERIFY( "sample" >= test );
	UT_VERIFY( "sample" <= test );
	UT_VERIFY( "samplebla" > test );
	UT_VERIFY( "sam" < test );

	String test2 = "dumb";
	UT_VERIFY( test != test2 );

	String test3 = "dumb";
	UT_VERIFY( test3 == test2 );
	UT_VERIFY( test3 == test3 );
	UT_VERIFY( test2 == test2 );
	UT_VERIFY( test2 >= test3 );
	UT_VERIFY( test2 <= test3 );
	UT_VERIFY( test >= test2 );
	UT_VERIFY( test > test2 );
	UT_VERIFY( test >= test3 );
	UT_VERIFY( test > test3 );

	UT_VERIFY( test3[0] == 'd' );
	UT_VERIFY( test2[2] == 'm' );

	test2[0] = 'D';
	UT_VERIFY( test2 == "Dumb" );
	test2[2] = 'M';
	UT_VERIFY( test2 == "DuMb" );

	String test4 = "";
	test4 += "bla";
	UT_VERIFY( test4 == "bla" );
	test4 += "";
	UT_VERIFY( test4 == "bla" );
	test4 += " ";
	UT_VERIFY( test4 == "bla " );
	test4 += "xxx";
	UT_VERIFY( test4 == "bla xxx" );
	test4.clear();
	UT_VERIFY( test4 == "" );
	test4.assign("moo");
	UT_VERIFY( test4 == "moo" );
	test4 += "xxx";
	UT_VERIFY( test4 == "mooxxx" );
	test4.assign("foo");
	UT_VERIFY( test4 == "foo" );

	test = "sample";
	test2 = "string";
	UT_VERIFY( (test + test2) == "samplestring" );

	test2 += test;
	UT_VERIFY( test2 == "stringsample" );
	test += test2;
	UT_VERIFY( test == "samplestringsample" );

	test.clear();
	test2.clear();
	UT_VERIFY( (test + test2) == "" );

	test3.clear();
	test = "sample ";
	test2 = "string ";
	test3 = "with no meanings";
	UT_VERIFY( (test + test2 + test3) == "sample string with no meanings" );
	UT_VERIFY( (test3 + test2 + test) == "with no meaningsstring sample " );
	UT_VERIFY( (test + test3 + test2) == "sample with no meaningsstring " );

	test3.clear();
	test3 = String("a") + "b" + String("c") + String("d") + "e" + String("f");
	UT_VERIFY( test3 == "abcdef" );
}

UT_FUNC(StringFind, "Test string find")
{
	String s("Sample string");
	UT_VERIFY( s.find("ample") == 1 );
	UT_VERIFY( s.find("XXX") == String::npos );
	UT_VERIFY( s.find('e', 0) == 5 );
}

UT_FUNC(StringSubstr, "Test string substr")
{
	String s("Sample string");
	UT_VERIFY( s.substr(6) == " string" );
	UT_VERIFY( s.substr(0, 6) == "Sample" );
	UT_VERIFY( s.substr(1, 5) == "ample" );

	String tmp(s.substr(7));
	UT_VERIFY( tmp == "string" );
}

UT_FUNC(StringPrintf, "Test string printf")
{
	String s;
	s.printf("Sample %s", "demo");
	UT_VERIFY( s == "Sample demo" );
	s.printf("Sample number %i", 5);
	UT_VERIFY( s == "Sample number 5" );

	s.clear();
	UT_VERIFY( s == "" );

	s.reserve(20);
	s.printf("Sample number %i", 5);
	UT_VERIFY( s == "Sample number 5" );
	UT_VERIFY( s.length() == 15 );
	UT_VERIFY( s.capacity() == 20 );

	s.printf("");
	UT_VERIFY( s.length() == 0 );
	UT_VERIFY( s.capacity() == 20 );
	s.clear();
	UT_VERIFY( s.capacity() == 0 );
}

#include <string>
UT_FUNC(StringComparison, "Test std::string comparison")
{
	String s;
	std::string s1;
	UT_VERIFY( s.length() == s1.length() );
	UT_VERIFY( s.capacity() == s1.capacity() );

	s = "foo";
	s1 = "foo";
	UT_VERIFY( s == s1.c_str() );

	UT_VERIFY( s.find("oo") == s1.find("oo") );
	UT_VERIFY( s.find("foo") == s1.find("foo") );
	UT_VERIFY( s.find("o") == s1.find("o") );
	UT_VERIFY( s.find('o', 0) == s1.find('o', 0) );

	s = "Sample string";
	// s1 is type of std::string
	s1 = "Sample string";
	String stmp = s1.substr(6).c_str();
	UT_VERIFY( s.substr(6) == stmp );

	std::string s2;
	s1.clear();
	s2.clear();
	String ss;
	ss.clear();
	s.clear();
	UT_VERIFY( String((s1 + s2).c_str()) == (ss + s) );

	s1.clear();
	s2.clear();

	s1.append(3, 'c');
	s2.append(3, 'c');

	UT_VERIFY( s1 == s2.c_str() );

	s1 = "sample str";
	s2 = "sample str";

	s1.append(1, 'C');
	s2.append(1, 'C');

	UT_VERIFY( s1 == s2.c_str() );

	s1.clear();
	s2.clear();

	s1.assign("Sample string", 5);
	s2.assign("Sample string", 5);

	UT_VERIFY( s1 == s2.c_str() );
}
