#include <edelib/Directory.h>
#include "UnitTest.h"
#include <stdio.h>

using namespace edelib;

UT_FUNC(DirectoryFunctions, "Test Directory functions")
{
	UT_VERIFY( dir_exists("/") );
	UT_VERIFY( dir_readable(".") );
	UT_VERIFY( dir_writeable(".") );

	UT_VERIFY( dir_exists("/root") );
	UT_VERIFY( dir_readable("/root") == false );
	UT_VERIFY( dir_writeable("/root") == false );

	UT_VERIFY( dir_exists("/we/dont/have/this/directory") == false );
	UT_VERIFY( dir_readable("/we/dont/have/this/directory") == false );
	UT_VERIFY( dir_writeable("/we/dont/have/this/directory") == false );

	UT_VERIFY( dir_exists("Jamfile") == false );
	UT_VERIFY( dir_exists("../Jamfile") == false );
	UT_VERIFY( dir_exists("../../src/File.cpp") == false );
	UT_VERIFY( dir_writeable("../../src/File.cpp") == false );
	UT_VERIFY( dir_exists("../edelib") );

	UT_VERIFY( dir_home() != "" );
	UT_VERIFY( dir_separator() == "/" );
	UT_VERIFY( dir_current() != "" );
}
