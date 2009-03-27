#include <edelib/File.h>
#include <string.h>
#include "UnitTest.h"

EDELIB_NS_USE

UT_FUNC(File, "Test File")
{
#if 0
	UT_VERIFY(file_exists("Jamfile") == true);
	UT_VERIFY(file_exists("../") == false);
#endif
	
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
#if 0
	UT_VERIFY(file_writeable("Jamfile") == true);
	UT_VERIFY(file_writeable("../edelib/Version.h") == true);
	UT_VERIFY(file_writeable("") == false);
	UT_VERIFY(file_writeable("/xxx/fff/bbb/ggg") == false);
	UT_VERIFY(file_readable("/xxx/fff/bbb/ggg") == false);
	UT_VERIFY(file_readable("/dev/this/is/not/file") == false);
	UT_VERIFY(file_exists("../") == false);
#endif

	UT_VERIFY( file_path("gcc") == "/usr/bin/gcc" );
	UT_VERIFY( file_path("gdb") == "/usr/bin/gdb" );
	UT_VERIFY( file_path("/bin/mkdir") == "/bin/mkdir" );
	UT_VERIFY( file_path("mkdir", true) == "/bin/mkdir" );
	UT_VERIFY( file_path("mv", true) == "/bin/mv" );
	UT_VERIFY( file_path("ls", true) == "/bin/ls" );

	UT_VERIFY( file_path("this-should-not-exists") == "" );
	UT_VERIFY( file_path("") == "" );
	UT_VERIFY( file_path("x") == "" );

#if 0
	UT_VERIFY( file_executable("run_tests") == true );
	UT_VERIFY( file_executable("file.cpp") == false );
	UT_VERIFY( file_executable("/bin/ls") == true );
	UT_VERIFY( file_executable("/") == false );
#endif
}

UT_FUNC(FileOperations, "Test File operations")
{
	File f;
	UT_VERIFY( f.open("demo-file.txt", FIO_WRITE) == true );
	f.printf("This is demo number %i\n", 1);
	f.close();

#if 0
	UT_VERIFY( file_exists("demo-file.txt") == true );
#endif
	UT_VERIFY( file_copy("demo-file.txt", "demo-file2.txt") == true );

#if 0
	UT_VERIFY( file_exists("demo-file2.txt") == true );
#endif

	// assertion will be trigered if one of above operations failed
	
	char buff[128];

	UT_VERIFY( f.open("demo-file2.txt") == true );
	f.readline(buff, 128);
	UT_VERIFY( strcmp(buff, "This is demo number 1\n") == 0 );
	f.close();

	UT_VERIFY( file_rename("demo-file2.txt", "dd2.txt") == true );
#if 0
	UT_VERIFY( file_exists("dd2.txt") == true );
	UT_VERIFY( file_exists("demo-file2.txt") != true );
#endif

	UT_VERIFY( file_remove("dd2.txt") == true );
#if 0
	UT_VERIFY( file_exists("dd2.txt") != true );
#endif

	UT_VERIFY( file_remove("demo-file.txt") == true );
#if 0
	UT_VERIFY( file_exists("demo-file.txt") != true );
#endif
}
