#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <assert.h>

#include "UnitTest.h"

#define SAFE_FREE(obj)    if(obj) { free(obj); obj = NULL; }
#define SAFE_DELETE(obj)  if(obj) { delete obj; obj = NULL; }

static char output_buf[1024];

void pretty_output(FILE* out, int num, UnitTest* t, double elapsed) {
	int len;

	snprintf(output_buf, sizeof(output_buf), "Test %2i: %s [%s]", num, t->name(), t->description());
	len = strlen(output_buf);

	fprintf(out, output_buf);

	while (len < 90) {
		putc(' ', out);
		++len;
	}

	if (!t->failed()) {
		fprintf(out, " OK (%g)\n", elapsed);
	} else {
		fprintf(out, " FAILED!\n");
		/* display reasons */
		if (t->msglist_size()) {
			for (const UTMsgList* ml = t->msglist(); ml; ml = ml->next) {
				if (ml->file)
					fprintf(out, "   '%s' : %s(%lu)\n", ml->msg, ml->file, ml->line);
				else
					fprintf(out, "   %s\n", ml->msg);
			}
		}

		fprintf(out, "\n");
	}

	/* must be done or external tools won't catch correctly stdin/stderr combination */
	fflush(out);
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

	/* sanity checks */
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

int UnitTestSuite::run(bool verbose) {
	if (!ntests)
		return 1;

	unsigned int i = 1;
	int passed = 0;
	int failed = 0;

	clock_t start_time;
	double  elapsed = 0, total_elapsed = 0;

	for (UTList* t = tlist->first; t; t = t->next, i++) {
		start_time = clock();
		t->test->execute();
		elapsed = double(clock() - start_time) / CLOCKS_PER_SEC;
		total_elapsed += elapsed;

		if (t->test->failed()) {
			failed++;
			pretty_output(stderr, i, t->test, elapsed);
		} else {
			passed++;

			if (verbose)
				pretty_output(stdout, i, t->test, elapsed);
		}
	}

	if (verbose) {
		printf("\n-------------------------------------------------------\n");
		printf("Tests: %i    Passed: %i    Failed: %i    (time: %g)\n", ntests, passed, failed, total_elapsed);
	}

	return (failed > 0);
}
