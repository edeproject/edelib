
#include <edelib/Config.h>
#include <stdio.h>

#include "Utest.h"

using namespace edelib;

int main()
{
	Config c;
	c.load("ede.conf");
	TEST_EQUAL(c, true);

	char buff[128];
	TEST_EQUAL(c.get("Desktop", "Color", buff, sizeof(buff)-1), true);
	TEST_STR_EQUAL(buff, "RGB(76,108,178)");
	int dummy;
	TEST_EQUAL(c.get("Panel", "AutoHide", dummy, 33), true);
	TEST_EQUAL(dummy, 0);
	TEST_EQUAL(c.get("Mouse", "Thresh", dummy, 33), true);
	TEST_EQUAL(dummy, 4);

	TEST_EQUAL(c.get("IconManager", "Label Fontsize", dummy, 33), true);
	TEST_EQUAL(dummy, 10);

	float dummy2;
	TEST_EQUAL(c.get("Screen", "Pattern", dummy2, 33.0), true);
	//TEST_EQUAL(dummy2, 3.4);

	TEST_NOTEQUAL(c.get("Mojo", "Jojo", dummy, 33), true);

	return 0;
}
