#include <edelib/List.h>
#include <list>
#include <stdlib.h>
#include <time.h>
#include "timer.hpp"
#include <iostream>

bool cmp_reverse(const unsigned int& v1, const unsigned int& v2) {
	return v1 > v2;
}

template <typename Container>
void test_sort(Container& container, const char* name, unsigned int* array, unsigned int sz) {
	for(unsigned int i = 0; i < sz; i++) {
		container.push_back(array[i]);
	}

	boost::timer tim;
	tim.restart();
	container.sort(cmp_reverse);
	container.sort();
	std::cout << "test " << name << " sort: " << tim.elapsed() << std::endl;
}

void test_comb_sort(edelib::list<unsigned int>& container, unsigned int* array, unsigned int sz) {
	for(unsigned int i = 0; i < sz; i++)
		container.push_back(array[i]);

	boost::timer tim;
	tim.restart();
	container.comb_sort(cmp_reverse);
	container.comb_sort();
	std::cout << "test combsort sort: " << tim.elapsed() << std::endl;
}

int main() {
	srand(time(0));

	//unsigned int array[150];
	//unsigned int array[5000];
	unsigned int array[10000];
	unsigned int sz = sizeof(array)/sizeof(unsigned int);

	for(unsigned int i = 0; i < sz; i++)
		array[i] = rand() % 50;

	edelib::list<unsigned int> elst;
	std::list<unsigned int> slst;

	test_sort(elst, "edelib::list", array, sz);
	test_sort(slst, "std::list", array, sz);

	elst.clear();
	test_comb_sort(elst, array, sz);

	return 0;
}
