#include <edelib/StrUtil.h>
#include <edelib/String.h>
#include <edelib/List.h>
#include <edelib/Vector.h>
#include "LinkedList.h"
#include "timer.hpp"
#include <stdio.h>
#include <list>
#include <iostream>

#define WHAT "</"

using namespace edelib;

void test_vector(const char* txt) {
	boost::timer tim;
	tim.restart();

	vector<String> vs;
	stringtok(vs, txt, WHAT);
	int sz = vs.size();

	vector<String>::iterator it = vs.begin();
	while(it != vs.end()) {
		(*it) += 'a';
		++it;
	}

	std::cout << "test_vector: " << tim.elapsed() << " sz: " << sz << std::endl;
}

void test_std_list(const char* txt) {
	boost::timer tim;
	tim.restart();

	std::list<String> vs;
	stringtok(vs, txt, WHAT);
	int sz = vs.size();

	std::list<String>::iterator it = vs.begin();
	while(it != vs.end()) {
		(*it) += 'a';
		++it;
	}

	std::cout << "test_std_list: " << tim.elapsed() << " sz: " << sz << std::endl;
}

void test_edelib_list(const char* txt) {
	boost::timer tim;
	tim.restart();

	List<String> vs;
	stringtok(vs, txt, WHAT);
	int sz = vs.size();

	List<String>::iterator it = vs.begin();
	while(it != vs.end()) {
		(*it) += 'a';
		++it;
	}


	std::cout << "test_edelib_list: " << tim.elapsed() << " sz: " << sz << std::endl;
}


void test_linked_list(const char* txt) {
	boost::timer tim;
	tim.restart();

	LinkedList<String> vs;
	stringtok(vs, txt, WHAT); 
	int sz = vs.size();

	LinkedList<String>::Node* n = vs.begin();
	while(n) {
		n->value += 'a';
		n = n->next;
	}

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

	for(int i = 0; i < 20; i++) {
	test_vector(buff);
	test_std_list(buff);
	test_edelib_list(buff);
	test_linked_list(buff);
	puts("--------------------------------");
	}

	delete [] buff;
	return 0;
}
