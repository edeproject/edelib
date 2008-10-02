#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>

#include "UnitTest.h"

#define SAFE_FREE(obj)    if(obj) { free(obj); obj = NULL; }
#define SAFE_DELETE(obj)  if(obj) { delete obj; obj = NULL; }

static void cdash_dump_test_list(const UTList* lst) {
	puts("   <TestList>");

	for(UTList* t = lst->first; t; t = t->next) {
		printf("     <Test>%s</Test>", t->test->name());
		if(t) 
			puts("");
	}

	puts("   </TestList>");
}

static void cdash_dump_test_status(UnitTest* test, double time_elapsed) {
	const char* st = NULL;

	if(test->failed())
		st = "failed";
	else
		st = "passed";

	printf("     <Test Status=\"%s\">\n", st);
	printf("      <Name>%s</Name>\n", test->name());
	printf("      <Path>.</Path>\n");
	printf("      <FullName>%s</FullName>\n", test->name());
	printf("      <FullCommandLine>./run_tests</FullCommandLine>\n");
	printf("      <Results>\n");
	printf("       <NamedMeasurement type=\"numeric/double\" name=\"Execution Time\"><Value>%f</Value></NamedMeasurement>\n", time_elapsed);
	printf("       <NamedMeasurement type=\"text/string\" name=\"Completion Status\"><Value>Completed</Value></NamedMeasurement>\n");
	printf("       <NamedMeasurement type=\"text/string\" name=\"Command Line\"><Value>run_tests</Value></NamedMeasurement>\n");
	printf("       <Measurement>\n");
	printf("        <Value></Value>\n");
	printf("       </Measurement>\n");
	printf("      </Results>\n");
	printf("     </Test>\n");
}

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

void UnitTestSuite::run(bool verbose, bool cdash_output) {
	if (!ntests)
		return;

	unsigned int i = 1;
	int passed = 0;
	int failed = 0;

	clock_t start_time;
	double  elapsed = 0, total_elapsed = 0;

	if(cdash_output)
		cdash_dump_test_list(tlist);

	for (UTList* t = tlist->first; t; t = t->next, i++) {
		start_time = clock();
		t->test->execute();
		elapsed = double(clock() - start_time) / CLOCKS_PER_SEC;
		total_elapsed += elapsed;

		if (cdash_output)
			cdash_dump_test_status(t->test, elapsed);

		if (t->test->failed()) {
			failed++;

			if (!cdash_output) {
				printf("Test %2i: %20s (%-30s): %30s (%g)\n", 
						i, t->test->name(), t->test->description(), "FAILED!", elapsed);
			}

			if (t->test->msglist_size() && !cdash_output) {
				for (const UTMsgList* ml = t->test->msglist(); ml; ml = ml->next) {
					if (ml->file)
						printf("   '%s' : %s(%lu)\n", ml->msg, ml->file, ml->line);
					else
						printf("   %s\n", ml->msg);
				}
			}

			if (!cdash_output)
				printf("\n");

		} else {
			passed++;

			if (verbose && !cdash_output) {
				printf("Test %2i: %20s (%-30s): %30s (%g)\n", 
						i, t->test->name(), t->test->description(), "Success", elapsed);
			}
		}
	}

	if (verbose && !cdash_output) {
		printf("\n-------------------------------------------------------\n");
		printf("Tests: %i    Passed: %i    Failed: %i    (time: %g)\n", ntests, passed, failed, total_elapsed);
	}
}
