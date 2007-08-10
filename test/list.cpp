#include <edelib/List.h>
#include "UnitTest.h"
#include <stdio.h>

using namespace edelib;

UT_FUNC(ListBasicTest, "Test basic list functions")
{
	list<int> ls;
	list<int>::iterator it;

	UT_VERIFY( ls.begin() == ls.end() );

	UT_VERIFY( ls.size() == 0 );
	ls.push_back(223);
	UT_VERIFY( ls.size() == 1 );
	ls.push_back(332);
	UT_VERIFY( ls.size() == 2 );

	it = ls.begin();
	UT_VERIFY( (*it) == 223 );
	++it;
	UT_VERIFY( (*it) == 332 );

	UT_VERIFY( ls.empty() != true );

	ls.clear();
	UT_VERIFY( ls.size() == 0 );
	UT_VERIFY( ls.empty() == true );
	UT_VERIFY( ls.begin() == ls.end() );
}

UT_FUNC(ListTestPushFront, "Test list push_front")
{
	list<int> ls;
	list<int>::iterator it;

	ls.push_back(3);
	it = ls.begin();
	UT_VERIFY( (*it) == 3 );

	ls.push_front(34);
	it = ls.begin();
	UT_VERIFY( (*it) == 34 );

	ls.push_front(3);
	ls.push_front(4);
	ls.push_front(6);
	ls.push_front(7);
	ls.push_front(8);
	ls.push_front(20);

	UT_VERIFY( ls.size() == 8 );

	it = ls.begin();
	UT_VERIFY( (*it) == 20 );

	ls.clear();
	UT_VERIFY( ls.empty() == true );
}

UT_FUNC(ListTestInsert, "Test list insert")
{
	list<int> ls;
	list<int>::iterator it;

	it = ls.begin();
	ls.insert(it, 4);
	UT_VERIFY( ls.size() == 1 );
	it = ls.begin();
	UT_VERIFY( (*it) == 4 );

	it = ls.end();
	ls.insert(it, 14);
	UT_VERIFY( ls.size() == 2 );
	it = ls.begin();
	++it;
	UT_VERIFY( (*it) == 14 );

	ls.push_front(33);
	ls.push_front(44);
	ls.push_front(55);
	it = ls.begin();
	ls.insert(it, 666);

	it = ls.begin();
	UT_VERIFY( (*it) == 666 );
	UT_VERIFY( ls.size() == 6 );

	++it; ++it;
	UT_VERIFY( (*it) == 44 );
}

UT_FUNC(ListTestErase, "Test list erase")
{
	list<int> ls;
	list<int>::iterator it;
	ls.push_back(23);
	it = ls.begin();
	ls.erase(it);

	UT_VERIFY( ls.size() == 0 );
	ls.push_back(3);
	ls.push_back(23);
	ls.push_back(35);
	it = ls.begin();
	++it;
	it = ls.erase(it);

	UT_VERIFY( (*it) == 35 );

	ls.clear();
	ls.push_back(4);
	ls.push_back(5);
	ls.push_back(6);

	ls.push_front(3);
	ls.push_front(2);
	ls.push_front(1);

	it = ls.begin();
	++it;
	UT_VERIFY( (*it) == 2 );
	it = ls.erase(it);

	// erase advances iterator by one
	UT_VERIFY( (*it) == 3 );
}

#include <list>
UT_FUNC(ListComparison, "Test std::list comparison")
{
	list<int> els;
	list<int>::iterator eit;

	// uh, remove that define !!!
	std::List<int> sls;
	std::List<int>::iterator sit;

	for(int i = 0; i < 10; i++) {
		els.push_back(i);
		sls.push_back(i);
	}

	UT_VERIFY( els.size() == sls.size() );

	eit = els.begin();
	sit = sls.begin();
	for(unsigned int i = 0; i < els.size(); i++) {
		UT_VERIFY( (*eit) == (*sit) );
		++eit;
		++sit;
	}

	els.clear();
	sls.clear();
	UT_VERIFY( els.size() == sls.size() );

	for(int i = 0; i < 10; i++) {
		els.push_back(i);
		sls.push_back(i);
	}

	for(int i = 10; i >= 0; i--) {
		els.push_front(i);
		sls.push_front(i);
	}

	UT_VERIFY( els.size() == sls.size() );

	eit = els.begin();
	sit = sls.begin();
	for(unsigned int i = 0; i < els.size(); i++) {
		UT_VERIFY( (*eit) == (*sit) );
		++eit;
		++sit;
	}

	// compare against end()
	eit = els.begin();
	sit = sls.begin();
	for(; eit != els.end(); ) {
		UT_VERIFY( (*eit) == (*sit) );
		++eit;
		++sit;
	}

	// compare against std::list end()
	eit = els.begin();
	sit = sls.begin();
	for(; sit != sls.end(); ) {
		UT_VERIFY( (*eit) == (*sit) );
		++eit;
		++sit;
	}
	
	// check erase's advance
	eit = els.begin();
	sit = sls.begin();
	++eit; ++eit;
	++sit; ++sit;

	eit = els.erase(eit);
	sit = sls.erase(sit);

	UT_VERIFY( (*eit) == (*sit) );
}
