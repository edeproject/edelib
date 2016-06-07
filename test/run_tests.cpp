#include <string.h>
#include "UnitTest.h"

int main(int argc, char** argv) {
	bool verbose = true;

	if(argv[1] && strcmp(argv[1], "--silent") == 0)
		verbose = false;

	return UnitTestSuite::global().run(verbose);
}
