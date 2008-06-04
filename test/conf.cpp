#include <edelib/Config.h>
#include <edelib/File.h>
#include <edelib/Missing.h>
#include <string.h>
#include "UnitTest.h"

#define CCHARP(str)           ((const char*)str)
#define STR_EQUAL(str1, str2) (strcmp(CCHARP(str1), CCHARP(str2)) == 0)

using namespace edelib;

UT_FUNC(ConfigTest, "Test Config class")
{
	Config c;
	c.load("ede.conf");
	if(c) {
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

		bool dummy3;
		UT_VERIFY(c.get("Panel", "RunBrowser", dummy3, false) == true);
		UT_VERIFY( dummy3 == true );
		UT_VERIFY(c.get("Panel", "RunBrowser2", dummy3, false) == true);
		UT_VERIFY( dummy3 == true );
		UT_VERIFY(c.get("Panel", "RunBrowser3", dummy3, false) == true);
		UT_VERIFY( dummy3 == true );
		UT_VERIFY(c.get("Panel", "RunBrowser4", dummy3, false) == true);
		UT_VERIFY( dummy3 == true );
		UT_VERIFY(c.get("Panel", "RunBrowser5", dummy3, true) == true);
		UT_VERIFY( dummy3 == false );
		UT_VERIFY(c.get("Screen", "RadioPattern", dummy3, true) == true);
		UT_VERIFY( dummy3 == false );

		// section/key exist
		UT_VERIFY( c.exist("Panel") == true );
		UT_VERIFY( c.exist("IconManager") == true );
		UT_VERIFY( c.exist("Mouse") == true );
		UT_VERIFY( c.exist("Screen") == true );

		UT_VERIFY( c.key_exist("Panel", "RunBrowser") == true );
		UT_VERIFY( c.key_exist("IconManager", "Label Fontsize") == true );
		UT_VERIFY( c.key_exist("Mouse", "Thresh") == true );
		UT_VERIFY( c.key_exist("Screen", "Pattern") == true );

		UT_VERIFY( c.key_exist("Screen", "Pattern None") == false );
		UT_VERIFY( c.key_exist("ScreenXXX", "Pattern None") == false );
		UT_VERIFY( c.key_exist("Screen", "") == false );

		// test write
		c.set("Panel", "AutoHide", 45);
		c.set("Mouse", "Thresh", 65);
		c.save(".ede.conf");

		if(c.load(".ede.conf")) {
			int dd;
			UT_VERIFY(c.get("Panel", "AutoHide", dd, 0) == true);
			UT_VERIFY(dd == 45);

			UT_VERIFY(c.get("Mouse", "Thresh", dd, 0) == true);
			UT_VERIFY(dd == 65);
		}
	} else {
		UT_FAIL("No ede.conf, but expected to be");
	}
}

UT_FUNC(ConfigTestLocaleRead, "Test Config locale read")
{
	Config c;
	c.load("test.desktop");
	if(!c)
		UT_FAIL("No test.desktop, but expected to be");
	else {
		char buff[128];
		edelib_setenv("LANG", "en_US", 1);
		UT_VERIFY(c.get_localized("Desktop Entry", "Name", buff, 128) == true);
		UT_VERIFY( STR_EQUAL(buff, "Home Folder") );
		edelib_unsetenv("LANG");

		// the real stuff :P
		edelib_setenv("LANG", "es_PE", 1);
		UT_VERIFY(c.get_localized("Desktop Entry", "Name", buff, 128) == true);
		UT_VERIFY( STR_EQUAL(buff, "Carpeta personal") );
		edelib_unsetenv("LANG");

		edelib_setenv("LANG", "et_EE", 1);
		UT_VERIFY(c.get_localized("Desktop Entry", "Name", buff, 128) == true);
		UT_VERIFY( STR_EQUAL(buff, "Kodukataloog") );
		edelib_unsetenv("LANG");

		edelib_setenv("LANG", "et_EE.iso885915", 1);
		UT_VERIFY(c.get_localized("Desktop Entry", "Name", buff, 128) == true);
		UT_VERIFY( STR_EQUAL(buff, "Kodukataloog") );
		edelib_unsetenv("LANG");

		edelib_setenv("LANG", "et_EE.utf8", 1);
		UT_VERIFY(c.get_localized("Desktop Entry", "Name", buff, 128) == true);
		UT_VERIFY( STR_EQUAL(buff, "Kodukataloog") );
		edelib_unsetenv("LANG");

		edelib_setenv("LANG", "eu_ES@euro", 1);
		UT_VERIFY(c.get_localized("Desktop Entry", "Name", buff, 128) == true);
		UT_VERIFY( STR_EQUAL(buff, "Etxea") );
		edelib_unsetenv("LANG");

		edelib_setenv("LANG", "eu_ES.utf8", 1);
		UT_VERIFY(c.get_localized("Desktop Entry", "Name", buff, 128) == true);
		UT_VERIFY( STR_EQUAL(buff, "EtxeaUTF8") );
		edelib_unsetenv("LANG");

		edelib_setenv("LANG", "eu_ES", 1);
		UT_VERIFY(c.get_localized("Desktop Entry", "Name", buff, 128) == true);
		UT_VERIFY( STR_EQUAL(buff, "Etxea") );
		edelib_unsetenv("LANG");

		edelib_setenv("LANG", "fr_BE", 1);
		UT_VERIFY(c.get_localized("Desktop Entry", "Name", buff, 128) == true);
		UT_VERIFY( STR_EQUAL(buff, "Dossier personnel") );
		edelib_unsetenv("LANG");

		edelib_setenv("LANG", "fr_LU@Latin", 1);
		UT_VERIFY(c.get_localized("Desktop Entry", "Name", buff, 128) == true);
		UT_VERIFY( STR_EQUAL(buff, "Dossier personnel Latin") );
		edelib_unsetenv("LANG");

		edelib_setenv("LANG", "fr_CH.utf8", 1);
		UT_VERIFY(c.get_localized("Desktop Entry", "Name", buff, 128) == true);
		UT_VERIFY( STR_EQUAL(buff, "Dossier personnel UTF8") );
		edelib_unsetenv("LANG");

		edelib_setenv("LANG", "en_US", 1);
	}
}

UT_FUNC(ConfigTestLocaleSave, "Test Config locale save")
{
	Config c;

	edelib_setenv("LANG", "fr_CH.utf8", 1);
	c.set_localized("Sample", "Key", "value fr_CH.utf8");
	edelib_unsetenv("LANG");

	edelib_setenv("LANG", "en_US", 1);
	c.set_localized("Sample", "Key", "value en_US");
	edelib_unsetenv("LANG");

	edelib_setenv("LANG", "et_EE", 1);
	c.set_localized("Sample", "Key", "value et_EE");
	edelib_unsetenv("LANG");

	edelib_setenv("LANG", "eu_ES@euro", 1);
	c.set_localized("Sample", "Key", "value eu_ES@euro");
	edelib_unsetenv("LANG");

	c.save("foo.conf");

	c.clear();

	if(!c.load("foo.conf")) {
		UT_FAIL("No foo.conf. WTF!!! I just wrote to it !!!");
		return;
	}

	char buff[128];
	edelib_setenv("LANG", "fr_CH.utf8", 1);
	UT_VERIFY( c.get_localized("Sample", "Key", buff, 128) == true );
	UT_VERIFY( STR_EQUAL(buff, "value fr_CH.utf8") );
	edelib_unsetenv("LANG");

	edelib_setenv("LANG", "en_US", 1);
	UT_VERIFY( c.get_localized("Sample", "Key", buff, 128) == true );
	UT_VERIFY( STR_EQUAL(buff, "value en_US") );
	edelib_unsetenv("LANG");

	edelib_setenv("LANG", "et_EE", 1);
	UT_VERIFY( c.get_localized("Sample", "Key", buff, 128) == true );
	UT_VERIFY( STR_EQUAL(buff, "value et_EE") );
	edelib_unsetenv("LANG");

	edelib_setenv("LANG", "eu_ES@euro", 1);
	UT_VERIFY( c.get_localized("Sample", "Key", buff, 128) == true );
	UT_VERIFY( STR_EQUAL(buff, "value eu_ES@euro") );
	edelib_unsetenv("LANG");

	// empty one
	UT_VERIFY( c.get_localized("Sample", "Key", buff, 128) == true );
	UT_VERIFY( STR_EQUAL(buff, "value en_US") );

	file_remove("foo.conf");
}


UT_FUNC(ConfigGetlineTest, "Test config_getline")
{
	FILE* f = fopen("test_configlongline.txt", "r");
	if(!f) {
		UT_FAIL("No test_configlongline.txt, but expected to be");
		return;
	}

	char* buff = NULL;
	int len = 0;

	// expect only 7 lines
	UT_VERIFY( config_getline(&buff, &len, f) != -1 );
	UT_VERIFY( STR_EQUAL(buff, "key1 = value1\n") );

	UT_VERIFY( config_getline(&buff, &len, f) != -1 );
	UT_VERIFY( STR_EQUAL(buff, "key2 = value2 value2 value2 value2 value2 value2 value2\n") );

	UT_VERIFY( config_getline(&buff, &len, f) != -1 );
	UT_VERIFY( STR_EQUAL(buff, "key3 = value3 value3 value3 value3\n") );

	UT_VERIFY( config_getline(&buff, &len, f) != -1 );
	UT_VERIFY( STR_EQUAL(buff, "key4 = value4 value4 value4 value4 value4 value4 value4 value4 value4 value4 value4 value4 value4 value4 value4\n") );

	UT_VERIFY( config_getline(&buff, &len, f) != -1 );
	UT_VERIFY( STR_EQUAL(buff, "key5 = value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5  value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5\n") );

	UT_VERIFY( config_getline(&buff, &len, f) != -1 );
	UT_VERIFY( STR_EQUAL(buff, "key6 = value 6\n") );

	UT_VERIFY( config_getline(&buff, &len, f) != -1 );
	UT_VERIFY( STR_EQUAL(buff, "key7 =\n") );

	fclose(f);
	delete [] buff;
}

UT_FUNC(ConfigGetlineTest2, "Test allocated config_getline")
{
	FILE* f = fopen("test_configlongline.txt", "r");
	if(!f) {
		UT_FAIL("No test_configlongline.txt, but expected to be");
		return;
	}

	// pre-allocate
	int len = 1024;
	char* buff = new char[len];

	// expect only 7 lines
	UT_VERIFY( config_getline(&buff, &len, f) != -1 );
	UT_VERIFY( STR_EQUAL(buff, "key1 = value1\n") );

	UT_VERIFY( config_getline(&buff, &len, f) != -1 );
	UT_VERIFY( STR_EQUAL(buff, "key2 = value2 value2 value2 value2 value2 value2 value2\n") );

	UT_VERIFY( config_getline(&buff, &len, f) != -1 );
	UT_VERIFY( STR_EQUAL(buff, "key3 = value3 value3 value3 value3\n") );

	UT_VERIFY( config_getline(&buff, &len, f) != -1 );
	UT_VERIFY( STR_EQUAL(buff, "key4 = value4 value4 value4 value4 value4 value4 value4 value4 value4 value4 value4 value4 value4 value4 value4\n") );

	UT_VERIFY( config_getline(&buff, &len, f) != -1 );
	UT_VERIFY( STR_EQUAL(buff, "key5 = value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5  value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5\n") );

	UT_VERIFY( config_getline(&buff, &len, f) != -1 );
	UT_VERIFY( STR_EQUAL(buff, "key6 = value 6\n") );

	UT_VERIFY( config_getline(&buff, &len, f) != -1 );
	UT_VERIFY( STR_EQUAL(buff, "key7 =\n") );

	fclose(f);
	delete [] buff;
}


UT_FUNC(ConfigGetlineTest3, "Test config_getline (File)")
{
	File f;
	if(!f.open("test_configlongline.txt")) {
		UT_FAIL("No test_configlongline.txt, but expected to be");
		return;
	}

	int len = 0;
	char* buff = NULL;

	// expect only 7 lines
	UT_VERIFY( config_getline(&buff, &len, &f) != -1 );
	UT_VERIFY( STR_EQUAL(buff, "key1 = value1\n") );

	UT_VERIFY( config_getline(&buff, &len, &f) != -1 );
	UT_VERIFY( STR_EQUAL(buff, "key2 = value2 value2 value2 value2 value2 value2 value2\n") );

	UT_VERIFY( config_getline(&buff, &len, &f) != -1 );
	UT_VERIFY( STR_EQUAL(buff, "key3 = value3 value3 value3 value3\n") );

	UT_VERIFY( config_getline(&buff, &len, &f) != -1 );
	UT_VERIFY( STR_EQUAL(buff, "key4 = value4 value4 value4 value4 value4 value4 value4 value4 value4 value4 value4 value4 value4 value4 value4\n") );

	UT_VERIFY( config_getline(&buff, &len, &f) != -1 );
	UT_VERIFY( STR_EQUAL(buff, "key5 = value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5  value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5 value5\n") );

	UT_VERIFY( config_getline(&buff, &len, &f) != -1 );
	UT_VERIFY( STR_EQUAL(buff, "key6 = value 6\n") );

	UT_VERIFY( config_getline(&buff, &len, &f) != -1 );
	UT_VERIFY( STR_EQUAL(buff, "key7 =\n") );

	delete [] buff;
}
