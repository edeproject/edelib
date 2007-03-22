#include <edelib/File.h>
#include "Utest.h"

using namespace edelib;

int main()
{
	TEST_EQUAL(file_writeable("Jamfile"), true);
	TEST_EQUAL(file_writeable("../edelib/Version.h"), true);
	TEST_EQUAL(file_writeable(""), false);
	TEST_EQUAL(file_writeable("/xxx/fff/bbb/ggg"), false);
	TEST_EQUAL(file_readable("/xxx/fff/bbb/ggg"), false);
	TEST_EQUAL(file_readable("/dev/this/is/not/file"), false);
	TEST_EQUAL(file_exists("../"), false);
	return 0;
}
