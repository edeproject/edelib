#include <unistd.h>
#include <edelib/FileTest.h>
#include <edelib/File.h>

#include "UnitTest.h"

EDELIB_NS_USE

UT_FUNC(FileTest, "Test file_test()")
{
	UT_VERIFY( file_test("Jamfile", FILE_TEST_EXISTS) );
	UT_VERIFY( file_test("Jamfile", FILE_TEST_IS_REGULAR) );
	UT_VERIFY( file_test("Jamfile", FILE_TEST_IS_READABLE) );
	UT_VERIFY( file_test("Jamfile", FILE_TEST_IS_WRITEABLE) );

	UT_VERIFY( file_test("Jamfile", FILE_TEST_EXISTS | FILE_TEST_IS_READABLE | FILE_TEST_IS_WRITEABLE) );

	UT_VERIFY( file_test("../", FILE_TEST_IS_DIR | 
				FILE_TEST_IS_READABLE |
				FILE_TEST_IS_WRITEABLE | 
				FILE_TEST_IS_EXECUTABLE));

	UT_VERIFY( file_test("../", FILE_TEST_IS_DIR | FILE_TEST_IS_REGULAR | FILE_TEST_EXISTS) == false );

	UT_VERIFY( file_test("this-does-not-exists", FILE_TEST_EXISTS) == false );
	UT_VERIFY( file_test("this-does-not-exists", FILE_TEST_IS_SOCKET) == false );

	UT_VERIFY( file_test("/usr/bin/mkdir", FILE_TEST_IS_SYMLINK) == true );
	UT_VERIFY( file_test(".", FILE_TEST_IS_SYMLINK) == false );

	UT_VERIFY( symlink("Jamfile", "Jamfile.lnk") == 0 );
	UT_VERIFY( file_test("Jamfile.lnk", FILE_TEST_IS_SYMLINK) == true );
	file_remove("Jamfile.lnk");
}
