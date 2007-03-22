#include <edelib/File.h>
#include "Utest.h"
#include <stdio.h>

using namespace edelib;

int main()
{
	TEST_EQUAL(file_exists("Jamfile"), true);
	TEST_EQUAL(file_exists("../"), false);
	
	File f;
	f.open("Jamfile");
	char buff[128];
	while(f.readline(buff, 128) >= 0 )
	{
		printf("%s", buff);	
	}
	f.close();

	TEST_EQUAL(f.name(), NULL);
	f.open(".foo.txt", FIO_WRITE);
	f.putch('c');
	f.close();
	f.open(".foo.txt");
	char cc = f.getch();
	TEST_EQUAL(cc, 'c');

	return 0;
}
