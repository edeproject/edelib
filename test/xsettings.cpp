#include <edelib/XSettingsCommon.h>
#include <stdio.h>
#include <string.h>
#include "UnitTest.h"

EDELIB_NS_USE;

UT_FUNC(XSettingsFuncsTest1, "Test XSETTINGS v_int")
{
	XSettingsSetting s1;
	s1.name = "Test/test";
	s1.type = XSETTINGS_TYPE_INT;
	s1.data.v_int = 3;

	XSettingsSetting* s2 = xsettings_setting_copy(&s1);
	UT_VERIFY( s2 != NULL );
	UT_VERIFY( xsettings_setting_equal(&s1, s2) == true );

	XSettingsSetting s3;
	s3.name = "Test/test";
	s3.type = XSETTINGS_TYPE_INT;
	s3.data.v_int = 1;

	UT_VERIFY( xsettings_setting_equal(&s1, &s3) == false );

	s3.name = "Test/test2";
	s3.type = XSETTINGS_TYPE_INT;
	s3.data.v_int = 3;

	UT_VERIFY( xsettings_setting_equal(&s1, &s3) == false );

	UT_VERIFY( xsettings_setting_len(&s1) == 24 );
	UT_VERIFY( xsettings_setting_len(s2)  == 24 );
	UT_VERIFY( xsettings_setting_len(&s3) == 24 );

	xsettings_setting_free(s2);
}

UT_FUNC(XSettingsFuncsTest2, "Test XSETTINGS v_string")
{
	XSettingsSetting s1;
	s1.name = "Test/test";
	s1.type = XSETTINGS_TYPE_STRING;
	s1.data.v_string = "dummy value";

	XSettingsSetting* s2 = xsettings_setting_copy(&s1);
	UT_VERIFY( s2 != NULL );
	UT_VERIFY( xsettings_setting_equal(&s1, s2) == true );

	XSettingsSetting s3;
	s3.name = "Test/test";
	s3.type = XSETTINGS_TYPE_STRING;
	s3.data.v_string = "dummyvalue";

	UT_VERIFY( xsettings_setting_equal(&s1, &s3) == false );

	s3.name = "Test/test2";
	s3.type = XSETTINGS_TYPE_STRING;
	s3.data.v_string = "dummy value";

	UT_VERIFY( xsettings_setting_equal(&s1, &s3) == false );

	UT_VERIFY( xsettings_setting_len(&s1) == 36 );
	UT_VERIFY( xsettings_setting_len(s2)  == 36 );
	UT_VERIFY( xsettings_setting_len(&s3) == 36 );

	xsettings_setting_free(s2);
}

UT_FUNC(XSettingsFuncsTest3, "Test XSETTINGS v_color")
{
	XSettingsColor col;
	col.red = 1;
	col.green = 2;
	col.blue = 3;
	col.alpha = 4;

	XSettingsSetting s1;
	s1.name = "Test/test";
	s1.type = XSETTINGS_TYPE_COLOR;
	s1.data.v_color = col;

	XSettingsSetting* s2 = xsettings_setting_copy(&s1);
	UT_VERIFY( s2 != NULL );
	UT_VERIFY( xsettings_setting_equal(&s1, s2) == true );

	col.red = 1;
	col.green = 2;
	col.blue = 0;
	col.alpha = 4;

	XSettingsSetting s3;
	s3.name = "Test/test";
	s3.type = XSETTINGS_TYPE_COLOR;
	s3.data.v_color = col;

	UT_VERIFY( xsettings_setting_equal(&s1, &s3) == false );

	s3.name = "Test/test2";
	s3.type = XSETTINGS_TYPE_COLOR;
	s3.data.v_color = s1.data.v_color;

	UT_VERIFY( xsettings_setting_equal(&s1, &s3) == false );

	UT_VERIFY( xsettings_setting_len(&s1) == 28 );
	UT_VERIFY( xsettings_setting_len(s2)  == 28 );
	UT_VERIFY( xsettings_setting_len(&s3) == 28 );

	xsettings_setting_free(s2);
}

UT_FUNC(XSettingsListTest, "Test XSETTINGS list")
{
	// xsettings_list_add expect already allocated data
	XSettingsSetting* s = new XSettingsSetting;
	s->type = XSETTINGS_TYPE_INT;
	s->data.v_int = 3;

	// xsettings_list_free expect allocated string data
	const char* key = "Test/test";
	s->name = new char[strlen(key) + 1];
	strcpy(s->name, key);

	XSettingsList* list = NULL;
	UT_VERIFY( xsettings_list_add(&list, s) == true );
	// adding same setting should not fill the list
	UT_VERIFY( xsettings_list_add(&list, s) == false );

	key = "Test/test2";
	s = new XSettingsSetting;
	s->type = XSETTINGS_TYPE_INT;
	s->data.v_int = 3;
	s->name = new char[strlen(key) + 1];
	strcpy(s->name, key);
	UT_VERIFY( xsettings_list_add(&list, s) == true );

	int len = 0;
	XSettingsList* iter = list;
	while(iter) {
		len++;
		iter = iter->next;
	}

	UT_VERIFY( len == 2 );

	UT_VERIFY( xsettings_list_find(list, "Test/test") != NULL );
	UT_VERIFY( xsettings_list_find(list, "Test/test2") != NULL );
	UT_VERIFY( xsettings_list_find(list, "Test/test3") == NULL );
	UT_VERIFY( xsettings_list_find(list, "") == NULL );

	UT_VERIFY( xsettings_list_remove(&list, "Test/test") == true );
	UT_VERIFY( xsettings_list_find(list, "Test/test") == NULL );

	len = 0;
	iter = list;
	while(iter) {
		len++;
		iter = iter->next;
	}

	UT_VERIFY( len == 1 );

	xsettings_list_free(list);
}
