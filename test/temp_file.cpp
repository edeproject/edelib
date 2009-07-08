#include <edelib/TempFile.h>
#include <edelib/FileTest.h>
#include "UnitTest.h"

EDELIB_NS_USE

UT_FUNC(TempFileTest, "Test TempFile")
{
	TempFile t, t2, t3;

	UT_VERIFY(t.create("foo-temp.XXXXXX") == true);
	UT_VERIFY(t == true);

	UT_VERIFY(file_test(t.name(), FILE_TEST_IS_REGULAR | FILE_TEST_IS_WRITEABLE));

	t.unlink();
	UT_VERIFY(file_test(t.name(), FILE_TEST_IS_REGULAR) == false);

	UT_VERIFY(t2.create("baz-tmp") == true);
	UT_VERIFY(t2 == true);
	UT_VERIFY(file_test(t2.name(), FILE_TEST_IS_REGULAR | FILE_TEST_IS_WRITEABLE));
	UT_VERIFY(t2.fstream() != 0);
	t2.set_auto_delete(true);

	UT_VERIFY(t3.create("/this/file/should/not/exists") == false);
	UT_VERIFY(t3 == false);
}

