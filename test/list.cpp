#include <edelib/List.h>
#include "UnitTest.h"
#include <stdio.h>

EDELIB_NS_USE

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

UT_FUNC(ListTestFrontBack, "Test list front()/back()")
{
	list<int> ls;
	ls.push_back(3);
	ls.push_back(5);
	ls.push_back(1);
	ls.push_back(0);

	UT_VERIFY( ls.front() == 3 );
	UT_VERIFY( ls.back() == 0 );

	ls.erase( --(ls.end()) );
	UT_VERIFY( ls.back() == 1 );

	ls.erase(ls.begin());
	UT_VERIFY( ls.front() == 5 );

	ls.clear();
	ls.push_back(10);
	UT_VERIFY( ls.front() == ls.back() );
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

UT_FUNC(ListTestEraseLoop, "Test list erase loop")
{
	int n_loop = 0, n_items = 0;

	list<int*> ls;
	ls.push_back(new int(1));
	ls.push_back(new int(2));
	ls.push_back(new int(3));
	ls.push_back(new int(4));
	ls.push_back(new int(5));
	ls.push_back(new int(6));
	ls.push_back(new int(7));

	n_items = ls.size();

	list<int*>::iterator it = ls.begin(), it_end = ls.end();
	while(it != it_end) {
		n_loop++;
		delete *it;
		it = ls.erase(it);
	}

	UT_VERIFY( n_loop == n_items );
}

UT_FUNC(ListTestEqual, "Test list equality operators")
{
	list<int> l1, l2;
	l1.push_back(2); l2.push_back(2);
	l1.push_back(2); l2.push_back(2);
	l1.push_back(3); l2.push_back(3);
	l1.push_back(3); l2.push_back(3);
	l1.push_back(10); l2.push_back(10);

	UT_VERIFY( l1 == l2 );

	l1.clear(); l2.clear();
	l1.push_back(1); l2.push_back(2);
	l1.push_back(2); l2.push_back(2);
	l1.push_back(3); l2.push_back(3);
	l1.push_back(3); l2.push_back(3);
	l1.push_back(10); l2.push_back(10);

	UT_VERIFY( l1 != l2 );

	l1.clear(); l2.clear();
	l1.push_back(2); l2.push_back(2);
	l1.push_back(2); l2.push_back(2);
	l1.push_back(3); l2.push_back(3);
	l1.push_back(3); l2.push_back(3);
	l1.push_back(1); l2.push_back(10);

	UT_VERIFY( l1 != l2 );

	l1.clear(); l2.clear();
	l1.push_back(2);
	l1.push_back(2); l2.push_back(2);
	l1.push_back(3); l2.push_back(3);
	l1.push_back(3); l2.push_back(3);
	l1.push_back(1); l2.push_back(10);

	UT_VERIFY( l1 != l2 );
}

static bool reverse_cmp(const int& v1, const int& v2) { return v1 > v2; }

UT_FUNC(ListTestSort, "Test list sort")
{
	list<int> ls;
	list<int>::iterator it, it_end;

	ls.push_back(2);
	ls.push_back(3);

	ls.sort();
	UT_VERIFY( ls.front() == 2 );
	UT_VERIFY( ls.back() == 3 );

	ls.clear();

	ls.push_back(5);
	ls.push_back(0);

	ls.sort();
	UT_VERIFY( ls.front() == 0 );
	UT_VERIFY( ls.back() == 5 );

	ls.clear();

	for(int i = 0; i < 20; i++)
		ls.push_back(i);

	ls.sort();
	
	it = ls.begin(), it_end = ls.end();
	for(int i = 0; it != it_end; i++, ++it)
		UT_VERIFY( *it == i );

	ls.clear();

	int array_random[] = {262, 362, 390, 345, 215, 289, 1, 167, 226, 77, 384, 390, 295, 2, 189, 321, 295, 104, 171, 550};
	int array_sorted[] = {1, 2, 77, 104, 167, 171, 189, 215, 226, 262, 289, 295, 295, 321, 345, 362, 384, 390, 390, 550};
	int array_rsorted[] = {550, 390, 390, 384, 362, 345, 321, 295, 295, 289, 262, 226, 215, 189, 171, 167, 104, 77, 2, 1};

	for(unsigned int i = 0; i < sizeof(array_random)/sizeof(int); i++)
		ls.push_back(array_random[i]);

	ls.sort();

	it = ls.begin();
	for(unsigned int i = 0; i < sizeof(array_sorted)/sizeof(int); i++, ++it)
		UT_VERIFY( *it == array_sorted[i] );

	ls.sort(reverse_cmp);

	it = ls.begin();
	for(unsigned int i = 0; i < sizeof(array_rsorted)/sizeof(int); i++, ++it)
		UT_VERIFY( *it == array_rsorted[i] );
}

struct Foo {
	int n;
};

// TODO: list<T*>::sort() bug
static bool ptr_sort(Foo* const& a, Foo* const& b) {
	return a->n > b->n;
}

UT_FUNC(ListTestPtrSort, "Test pointer list sort")
{
	list<Foo*> ls;
	Foo* f;

	for(int i = 0; i < 10; i++) {
		f = new Foo;
		f->n = i;
		ls.push_back(f);
	}

	ls.sort(ptr_sort);

	list<Foo*>::iterator it = ls.begin();
	for(int i = 9; i >= 0; i--, ++it) {
		UT_VERIFY( i == (*it)->n );
		delete *it;
	}
}

#include <list>
UT_FUNC(ListComparison, "Test std::list comparison")
{
	list<int> els;
	list<int>::iterator eit;

	std::list<int> sls;
	std::list<int>::iterator sit;

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

	// check sort
	els.clear();
	sls.clear();

	int array[] = {10, 3, 43, 34, 455, 455, 0, 0, 0, 1, 1, 1, 23, 120, 5, 234, 34, 78, 10, 12, 55, 123};
	for(unsigned int i = 0; i < sizeof(array)/sizeof(int); i++) {
		els.push_back(i);
		sls.push_back(i);
	}

	els.sort();
	sls.sort();

	UT_VERIFY( els.size() == sls.size() );
	eit = els.begin();
	sit = sls.begin();

	for(unsigned int i = 0; i < els.size(); i++, ++eit, ++sit)
		UT_VERIFY( *eit == *sit );
}
