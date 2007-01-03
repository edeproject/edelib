#include <edelib/File.h>
#include "Utest.h"

using namespace edelib;

int main()
{
	TEST_EQUAL(file_exists("Jamfile"), true);
	TEST_EQUAL(file_exists("../"), false);
	
	File f;
	f.open("Jamfile");
	char buff[128];
	int n;
	while(!f.eof())
	{
		n = f.readline(buff, 128);
		//printf("%i: %s", n, buff);
	}
	f.close();
	return 0;
}
