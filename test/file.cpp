#include <edelib/File.h>
#include "UnitTest.h"

using namespace edelib;

UT_FUNC(File, "Test File")
{
	UT_VERIFY(file_exists("Jamfile") == true);
	UT_VERIFY(file_exists("../") == false);
	
	File f;
	f.open("Jamfile");
	char buff[128];
	while(f.readline(buff, 128) >= 0 )
	{
	}
	f.close();

	UT_VERIFY(f.name() == NULL);
	f.open(".foo.txt", FIO_WRITE);
	f.putch('c');
	f.close();
	f.open(".foo.txt");
	char cc = f.getch();
	UT_VERIFY(cc == 'c');
}

UT_FUNC(FileFunctions, "Test File functions")
{
	UT_VERIFY(file_writeable("Jamfile") == true);
	UT_VERIFY(file_writeable("../edelib/Version.h") == true);
	UT_VERIFY(file_writeable("") == false);
	UT_VERIFY(file_writeable("/xxx/fff/bbb/ggg") == false);
	UT_VERIFY(file_readable("/xxx/fff/bbb/ggg") == false);
	UT_VERIFY(file_readable("/dev/this/is/not/file") == false);
	UT_VERIFY(file_exists("../") == false);
}
