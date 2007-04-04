#include <edelib/Util.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "UnitTest.h"

using namespace edelib;

#define STR_EQUAL(str1, str2) (str1 == str2)

UT_FUNC(XDGPathTest, "Test XDG paths")
{
	UT_VERIFY( STR_EQUAL(user_config_dir(), "~/.config/ede/") );
	UT_VERIFY( STR_EQUAL(user_data_dir(), "~/.local/share/ede/") );
	UT_VERIFY( STR_EQUAL(user_cache_dir(), "~/.cache/ede/") );

	setenv("XDG_CONFIG_HOME", "/dummy/place", 1);
	UT_VERIFY( STR_EQUAL(user_config_dir(), "/dummy/place/ede/") );

	setenv("XDG_CONFIG_HOME", "", 1);
	UT_VERIFY( STR_EQUAL(user_config_dir(), "~/.config/ede/") );
	unsetenv("XDG_CONFIG_HOME");

	setenv("EDE_CONFIG_HOME", "/dummy/place/with/slash/", 1);
	UT_VERIFY( STR_EQUAL(user_config_dir(), "/dummy/place/with/slash/ede/") );
	unsetenv("EDE_CONFIG_HOME");

	setenv("XDG_DATA_HOME", "", 1);
	UT_VERIFY( STR_EQUAL(user_data_dir(), "~/.local/share/ede/") );

	setenv("XDG_DATA_HOME", "/", 1);
	UT_VERIFY( STR_EQUAL(user_data_dir(), "/ede/") );

	setenv("XDG_DATA_HOME", "/baz", 1);
	UT_VERIFY( STR_EQUAL(user_data_dir(), "/baz/ede/") );
	unsetenv("XDG_DATA_HOME");

	setenv("EDE_DATA_HOME", "", 1);
	UT_VERIFY( STR_EQUAL(user_data_dir(), "~/.local/share/ede/") );

	setenv("EDE_DATA_HOME", "/", 1);
	UT_VERIFY( STR_EQUAL(user_data_dir(), "/ede/") );

	setenv("EDE_DATA_HOME", "/baz", 1);
	UT_VERIFY( STR_EQUAL(user_data_dir(), "/baz/ede/") );
	unsetenv("EDE_DATA_HOME");

	setenv("XDG_CACHE_HOME", "", 1);
	UT_VERIFY( STR_EQUAL(user_cache_dir(), "~/.cache/ede/") );

	setenv("XDG_CACHE_HOME", "/", 1);
	UT_VERIFY( STR_EQUAL(user_cache_dir(), "/ede/") );

	setenv("XDG_CACHE_HOME", "/home/foo", 1);
	UT_VERIFY( STR_EQUAL(user_cache_dir(), "/home/foo/ede/") );
	unsetenv("XDG_CACHE_HOME");

	setenv("EDE_CACHE_HOME", "", 1);
	UT_VERIFY( STR_EQUAL(user_cache_dir(), "~/.cache/ede/") );

	setenv("EDE_CACHE_HOME", "/", 1);
	UT_VERIFY( STR_EQUAL(user_cache_dir(), "/ede/") );

	setenv("EDE_CACHE_HOME", "/home/foo", 1);
	UT_VERIFY( STR_EQUAL(user_cache_dir(), "/home/foo/ede/") );
	unsetenv("EDE_CACHE_HOME");
}
