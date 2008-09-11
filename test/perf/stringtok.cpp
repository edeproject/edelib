#include <edelib/StrUtil.h>
#include <edelib/String.h>
#include <edelib/List.h>
#include <stdio.h>
#include <string.h>
#include <list>
#include <vector>
#include <iostream>

#include "LinkedList.h"
#include "timer.hpp"
#include "strsplit.h"

#define WHAT "</"

EDELIB_NS_USE

void test_vector(const char* txt) {
	boost::timer tim;
	tim.restart();

	std::vector<String> vs;
	stringtok(vs, txt, WHAT);
	int sz = vs.size();

	std::vector<String>::iterator it = vs.begin();
	while(it != vs.end()) {
		(*it) += 'a';
		++it;
	}

	std::cout << "test_std_vector: " << tim.elapsed() << " sz: " << sz << std::endl;
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

	list<String> vs;
	stringtok(vs, txt, WHAT);
	int sz = vs.size();

	list<String>::iterator it = vs.begin();
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

void test_strsplit(const char* txt) {
	boost::timer tim;
	tim.restart();

	unsigned int sz = 0;
	char** toks = strsplit(txt, WHAT, -1);
	for(int i = 0; toks[i]; i++)
		sz++;

	strsplit_free(toks);

	std::cout << "test_strsplit: " << tim.elapsed() << " sz: " << sz << std::endl;
}

void test_strtok(const char* txt) {
	boost::timer tim;
	tim.restart();

	unsigned int sz = 0;
	char* copy = strdup(txt);
	char* saveptr;

	char* tok = strtok_r(copy, WHAT, &saveptr);
	while(tok != NULL) {
		sz++;
		tok = strtok_r(NULL, WHAT, &saveptr);
	}

	free(copy);
	std::cout << "test_strtok_r: " << tim.elapsed() << " sz: " << sz << std::endl;
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
		test_strsplit(buff);
		test_strtok(buff);
		puts("--------------------------------");
	}

	delete [] buff;
	return 0;
}
