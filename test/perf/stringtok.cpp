#include <edelib/StrUtil.h>
#include <edelib/String.h>
#include <edelib/List.h>
#include <edelib/Vector.h>
#include "LinkedList.h"
#include "timer.hpp"
#include <stdio.h>
#include <iostream>

#define WHAT "</"

using namespace edelib;

void test_vector(const char* txt) {
	boost::timer tim;
	tim.restart();

	vector<String> vs;
	stringtok(vs, txt, WHAT);
	int sz = vs.size();

	std::cout << "test_vector: " << tim.elapsed() << " sz: " << sz << std::endl;
}

void test_list(const char* txt) {
	boost::timer tim;
	tim.restart();

	list<String> vs;
	stringtok(vs, txt, WHAT);
	int sz = vs.size();

	std::cout << "test_list: " << tim.elapsed() << " sz: " << sz << std::endl;
}

void test_linked_list(const char* txt) {
	boost::timer tim;
	tim.restart();

	LinkedList<String> vs;
	stringtok(vs, txt, WHAT); 
	int sz = vs.size();

	std::cout << "test_linked_list: " << tim.elapsed() << " sz: " << sz << std::endl;
}

int main() {
	FILE* f = fopen("asciidoc.html", "r");
	if(!f)
		return 1;
	long len;

	fseek(f, 0, SEEK_END);
	len = ftell(f);
	fseek(f, 0, SEEK_SET);

	char* buff = new char[len + 1];
	fread(buff, len, 1, f);
	fclose(f);

	test_vector(buff);
	test_list(buff);
	test_linked_list(buff);

	delete [] buff;
	return 0;
}
