#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "UnitTest.h"

#define SAFE_FREE(obj)    if(obj) { free(obj); obj = NULL; }
#define SAFE_DELETE(obj)  if(obj) { delete obj; obj = NULL; }

UnitTest::UnitTest(const char* name, const char* descr) {
	assert(name != NULL);
	assert(descr != NULL);
	tname = strdup(name);
	tdescr = strdup(descr);
	mlist = NULL;
	mnumber = 0;
	tfails = tpasses = 0;
}

UnitTest::~UnitTest() {
	SAFE_FREE(tname);
	SAFE_FREE(tdescr);

	if (!mlist)
		return;

	mlist = mlist->first;

	do {
		UTMsgList* tmp = mlist;
		mlist = mlist->next;

		SAFE_FREE(tmp->msg);
		SAFE_FREE(tmp->file);
		SAFE_DELETE(tmp);

		mnumber--;
	} while (mlist);

	assert(mnumber == 0);
}

void UnitTest::add_message(const char* msg, const char* fname, unsigned long line) {
	if (!msg)
		return;
	if (mlist) {
		mlist->next = new UTMsgList;
		mlist->next->first = mlist->first;
		mlist = mlist->next;
	} else {
		mlist = new UTMsgList;
		mlist->first = mlist;
	}

	mlist->next = NULL;
	mlist->msg = strdup(msg);
	if (fname)
		mlist->file = strdup(fname);
	else
		mlist->file = NULL;
	mlist->line = line;
	mnumber++;
}

const UTMsgList* UnitTest::msglist(void) {
	if (mlist)
		return mlist->first;
	return NULL;
}

void UnitTest::add_message(const char* msg) {
	add_message(msg, NULL, 0);
}

void UnitTest::verify(bool condition, const char* condstr, const char* fname, unsigned long line) {
	if (!condition) {
		add_message(condstr, fname, line);
		tfails++;
	} else
		tpasses++;
}

void UnitTest::execute(void) {

	try {
		run();
	} catch (...) {
		add_message("Unknown exception caught!");
	}
}

UnitTestSuite::UnitTestSuite() : ntests(0), tlist(NULL) {}

UnitTestSuite::~UnitTestSuite() {
	if (!tlist)
		return;

	tlist = tlist->first;
	do {
		UTList* tmp = tlist;
		tlist = tlist->next;
		if (tmp->alloc)
			SAFE_DELETE(tmp->test);
		SAFE_DELETE(tmp);
		ntests--;
	} while (tlist);

	// sanity checks
	assert(ntests == 0);
}

UnitTestSuite& UnitTestSuite::global(void) {
	static UnitTestSuite instance;
	return instance;
}

void UnitTestSuite::add(UnitTest* t, bool alloc) {
	if (tlist) {
		tlist->next = new UTList;
		tlist->next->first = tlist->first;
		tlist = tlist->next;
	} else {
		tlist = new UTList;
		tlist->first = tlist;
	}

	tlist->next = NULL;
	tlist->test = t;
	tlist->alloc = alloc;
	ntests++;
}

void UnitTestSuite::run(bool verbose) {
	if (!ntests)
		return;

	unsigned int i = 1;
	int passed = 0;
	int failed = 0;
	for (UTList* t = tlist->first; t; t = t->next, i++) {
		t->test->execute();

		if (t->test->failed()) {
			failed++;

			printf("Test %2i: %20s (%-30s): %30s\n", i, t->test->name(), t->test->description(), "FAILED!");
			if (t->test->msglist_size()) {
				for (const UTMsgList* ml = t->test->msglist(); ml; ml = ml->next) {
					if (ml->file)
						printf("   '%s' : %s(%lu)\n", ml->msg, ml->file, ml->line);
					else
						printf("   %s\n", ml->msg);
				}
			}
			printf("\n");
		} else {
			passed++;
			if (verbose)
				printf("Test %2i: %20s (%-30s): %30s\n", i, t->test->name(), t->test->description(), "Success");
		}
	}

	if (!verbose)
		return;

	printf("\n--------------------------------------------\n");
	printf("Tests: %i    Passed: %i    Failed: %i\n", ntests, passed, failed);
}
