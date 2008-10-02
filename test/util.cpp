#include <edelib/Util.h>
#include <edelib/Missing.h>
#include <edelib/String.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>  // getenv
#include "UnitTest.h"

EDELIB_NS_USE

#define STR_EQUAL(str1, str2) (str1 == str2)

#define STORE_ENV(name, to, ptr) \
do {                             \
	ptr = getenv(name);          \
	if(ptr)                      \
		to = ptr;                \
} while(0)

/*
 * Ironically, we will use edelib_setenv() for actual setting.
 * Hoping before this call, everything was passed fine ;-).
 *
 * Hey, these are test; I'm not expecting they save the world if
 * they crashes :-P
 */
#define RESTORE_ENV(name, from)               \
do {                                          \
	if(!from.empty())                         \
		edelib_setenv(name, from.c_str(), 1); \
} while(0)

UT_FUNC(XDGPathTest, "Test XDG paths")
{
	// save previous values
	char* p;
	String config_home, data_home, cache_home, data_dirs, config_dirs;

	STORE_ENV("XDG_CONFIG_HOME", config_home, p);
	STORE_ENV("XDG_DATA_HOME", data_home, p);
	STORE_ENV("XDG_CACHE_HOME", cache_home, p);
	STORE_ENV("XDG_DATA_DIRS", data_dirs, p);
	STORE_ENV("XDG_CONFIG_DIRS", config_dirs, p);

	edelib_setenv("XDG_CONFIG_HOME", "/dummy/place", 1);
	UT_VERIFY( STR_EQUAL(user_config_dir(), "/dummy/place") );
	edelib_unsetenv("XDG_CONFIG_HOME");

	edelib_setenv("XDG_DATA_HOME", "/", 1);
	UT_VERIFY( STR_EQUAL(user_data_dir(), "/") );

	edelib_setenv("XDG_DATA_HOME", "/baz", 1);
	UT_VERIFY( STR_EQUAL(user_data_dir(), "/baz") );
	edelib_unsetenv("XDG_DATA_HOME");

	edelib_setenv("XDG_CACHE_HOME", "/", 1);
	UT_VERIFY( STR_EQUAL(user_cache_dir(), "/") );

	edelib_setenv("XDG_CACHE_HOME", "/home/foo", 1);
	UT_VERIFY( STR_EQUAL(user_cache_dir(), "/home/foo") );
	edelib_unsetenv("XDG_CACHE_HOME");

	edelib_setenv("XDG_CACHE_HOME", "/home/foo/withslash/", 1);
	UT_VERIFY( STR_EQUAL(user_cache_dir(), "/home/foo/withslash") );
	edelib_unsetenv("XDG_CACHE_HOME");

	list<String> lst;
	edelib_setenv("XDG_DATA_DIRS", "/home/foo:/home/baz:/home/taz", 1);
	int ret = system_data_dirs(lst);

	list<String>::iterator it = lst.begin();

	UT_VERIFY( ret == 3 );
	UT_VERIFY( (*it) == "/home/foo" );
	++it;
	UT_VERIFY( (*it) == "/home/baz" );
	++it;
	UT_VERIFY( (*it) == "/home/taz" );

	edelib_unsetenv("XDG_DATA_DIRS");
	lst.clear();

	edelib_setenv("XDG_DATA_DIRS", "/home/foo", 1);
	ret = system_data_dirs(lst);
	it = lst.begin();

	UT_VERIFY( ret == 1 );
	UT_VERIFY( (*it) == "/home/foo" );

	edelib_unsetenv("XDG_DATA_DIRS");
	lst.clear();

	// in case of empty XDG_DATA_DIRS
	ret = system_data_dirs(lst);
	it = lst.begin();

	UT_VERIFY( ret == 2 );
	UT_VERIFY( (*it) == "/usr/local/share" );
	++it;
	UT_VERIFY( (*it) == "/usr/share" );

	lst.clear();

	edelib_setenv("XDG_CONFIG_DIRS", "/etc/myconf:/etc/yourconf:/usr/local/share/conf:/some/path/conf", 1);
	ret = system_config_dirs(lst);
	it = lst.begin();

	UT_VERIFY( ret == 4 );
	UT_VERIFY( (*it) == "/etc/myconf" );
	++it;
	UT_VERIFY( (*it) == "/etc/yourconf" );
	++it;
	UT_VERIFY( (*it) == "/usr/local/share/conf" );
	++it;
	UT_VERIFY( (*it) == "/some/path/conf" );

	edelib_unsetenv("XDG_CONFIG_DIRS");
	lst.clear();

	edelib_setenv("XDG_CONFIG_DIRS", "/etc/mydirconf", 1);
	ret = system_config_dirs(lst);
	it = lst.begin();

	UT_VERIFY( ret == 1 );
	UT_VERIFY( lst.size() == 1 );
	UT_VERIFY( (*it) == "/etc/mydirconf" );

	edelib_unsetenv("XDG_CONFIG_DIRS");
	lst.clear();

	// in case of empty XDG_CONFIG_DIRS
	ret = system_config_dirs(lst);
	it = lst.begin();

	UT_VERIFY( ret == 1 );
	UT_VERIFY( (*it) == "/etc/xdg" );

	RESTORE_ENV("XDG_CONFIG_HOME", config_home);
	RESTORE_ENV("XDG_DATA_HOME", data_home);
	RESTORE_ENV("XDG_CACHE_HOME", cache_home);
	RESTORE_ENV("XDG_DATA_DIRS", data_dirs);
	RESTORE_ENV("XDG_CONFIG_DIRS", config_dirs);
}

UT_FUNC(PathBuildTest, "Test path builders")
{
	String s = build_filename("foo", "baz", "taz");
	UT_VERIFY( s == "foo/baz/taz" );

	s = build_filename("/foo/baz////", "baz", "myfile.txt/");
	UT_VERIFY( s == "/foo/baz/baz/myfile.txt" );

	s = build_filename("home", "myfile.txt");
	UT_VERIFY( s == "home/myfile.txt" );

	s = build_filename("///home/", "/myfile.txt//");
	UT_VERIFY( s == "/home/myfile.txt" );

	s = build_dirname("/", "home", "mydir");
	UT_VERIFY( s == "/home/mydir/" );

	s = build_dirname("/", "/", "/demodir");
	UT_VERIFY( s == "/demodir/" );

	s = build_dirname("/", "///", "/demodir");
	UT_VERIFY( s == "/demodir/" );
}
