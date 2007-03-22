#include <edelib/Config.h>
#include <string.h>
#include "UnitTest.h"

#define CCHARP(str)           ((const char*)str)
#define STR_EQUAL(str1, str2) (strcmp(CCHARP(str1), CCHARP(str2)) == 0)

using namespace edelib;

UT_FUNC(ConfigTest, "Test Config class")
{
	Config c;
	c.load("ede.conf");
	if(c)
	{
		UT_VERIFY(c == true);
		char buff[128];
		UT_VERIFY(c.get("Desktop", "Color", buff, sizeof(buff)-1) == true);

		UT_VERIFY( STR_EQUAL(buff, "RGB(76,108,178)") );

		int dummy;
		UT_VERIFY(c.get("Panel", "AutoHide", dummy, 33) == true);
		UT_VERIFY(dummy == 0);
		UT_VERIFY(c.get("Mouse", "Thresh", dummy, 33) == true);
		UT_VERIFY(dummy == 4);
		UT_VERIFY(c.get("IconManager", "Label Fontsize", dummy, 33) == true);
		UT_VERIFY(dummy == 10);

		float dummy2;
		UT_VERIFY(c.get("Screen", "Pattern", dummy2, 33.0) == true);
		UT_VERIFY(c.get("Mojo", "Jojo", dummy, 33) == false);

		// test write
		c.set("Panel", "AutoHide", 45);
		c.set("Mouse", "Thresh", 65);
		c.save(".ede.conf");

		if(c.load(".ede.conf"))
		{
			int dd;
			UT_VERIFY(c.get("Panel", "AutoHide", dd, 0) == true);
			UT_VERIFY(dd == 45);

			UT_VERIFY(c.get("Mouse", "Thresh", dd, 0) == true);
			UT_VERIFY(dd == 65);
		}
	}
	else
	{
		UT_FAIL("No ede.conf, but expected to be");
	}
}
