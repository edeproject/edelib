#include <string.h>
#include "UnitTest.h"

int main(int argc, char** argv) {
	bool cdash_output = 0;
	if(argc == 2 && (strcmp(argv[1], "--cdash") == 0))
		cdash_output = 1;

	UnitTestSuite::global().run(1, cdash_output);
	return 0;
}
