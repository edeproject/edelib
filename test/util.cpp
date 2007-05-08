#include <edelib/Util.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "UnitTest.h"

using namespace edelib;

#define STR_EQUAL(str1, str2) (str1 == str2)

UT_FUNC(XDGPathTest, "Test XDG paths")
{
	puts(" *** XDGPathTest mess with some environment variables that are probably");
	puts(" *** used by the current environment. Maybe strange things can be occured later");

	UT_VERIFY( STR_EQUAL(user_config_dir(), "~/.config") );
	UT_VERIFY( STR_EQUAL(user_data_dir(), "~/.local/share") );
	UT_VERIFY( STR_EQUAL(user_cache_dir(), "~/.cache") );

	setenv("XDG_CONFIG_HOME", "/dummy/place", 1);
	UT_VERIFY( STR_EQUAL(user_config_dir(), "/dummy/place") );

	setenv("XDG_CONFIG_HOME", "", 1);
	UT_VERIFY( STR_EQUAL(user_config_dir(), "~/.config") );
	unsetenv("XDG_CONFIG_HOME");

	setenv("EDE_CONFIG_HOME", "/dummy/place/with/slash/", 1);
	UT_VERIFY( STR_EQUAL(user_config_dir(), "/dummy/place/with/slash") );
	unsetenv("EDE_CONFIG_HOME");

	setenv("XDG_DATA_HOME", "", 1);
	UT_VERIFY( STR_EQUAL(user_data_dir(), "~/.local/share") );

	setenv("XDG_DATA_HOME", "/", 1);
	UT_VERIFY( STR_EQUAL(user_data_dir(), "/") );

	setenv("XDG_DATA_HOME", "/baz", 1);
	UT_VERIFY( STR_EQUAL(user_data_dir(), "/baz") );
	unsetenv("XDG_DATA_HOME");

	setenv("EDE_DATA_HOME", "", 1);
	UT_VERIFY( STR_EQUAL(user_data_dir(), "~/.local/share") );

	setenv("EDE_DATA_HOME", "/", 1);
	UT_VERIFY( STR_EQUAL(user_data_dir(), "/") );

	setenv("EDE_DATA_HOME", "/baz", 1);
	UT_VERIFY( STR_EQUAL(user_data_dir(), "/baz") );
	unsetenv("EDE_DATA_HOME");

	setenv("XDG_CACHE_HOME", "", 1);
	UT_VERIFY( STR_EQUAL(user_cache_dir(), "~/.cache") );

	setenv("XDG_CACHE_HOME", "/", 1);
	UT_VERIFY( STR_EQUAL(user_cache_dir(), "/") );

	setenv("XDG_CACHE_HOME", "/home/foo", 1);
	UT_VERIFY( STR_EQUAL(user_cache_dir(), "/home/foo") );
	unsetenv("XDG_CACHE_HOME");

	setenv("XDG_CACHE_HOME", "/home/foo/withslash/", 1);
	UT_VERIFY( STR_EQUAL(user_cache_dir(), "/home/foo/withslash") );
	unsetenv("XDG_CACHE_HOME");

	setenv("EDE_CACHE_HOME", "", 1);
	UT_VERIFY( STR_EQUAL(user_cache_dir(), "~/.cache") );

	setenv("EDE_CACHE_HOME", "/", 1);
	UT_VERIFY( STR_EQUAL(user_cache_dir(), "/") );

	setenv("EDE_CACHE_HOME", "/home/foo", 1);
	UT_VERIFY( STR_EQUAL(user_cache_dir(), "/home/foo") );
	unsetenv("EDE_CACHE_HOME");

	vector<String> lst;
	setenv("XDG_DATA_DIRS", "/home/foo:/home/baz:/home/taz", 1);
	int ret = system_data_dirs(lst);

	UT_VERIFY(ret == 3);
	UT_VERIFY(lst[0] == "/home/foo");
	UT_VERIFY(lst[1] == "/home/baz");
	UT_VERIFY(lst[2] == "/home/taz");

	unsetenv("XDG_DATA_DIRS");
	lst.clear();

	setenv("XDG_DATA_DIRS", "/home/foo", 1);
	ret = system_data_dirs(lst);
	UT_VERIFY( ret == 1 );
	UT_VERIFY( lst[0] == "/home/foo" );

	unsetenv("XDG_DATA_DIRS");
	lst.clear();

	// in case of empty XDG_DATA_DIRS
	ret = system_data_dirs(lst);
	UT_VERIFY( ret == 2 );
	UT_VERIFY( lst[0] == "/usr/local/share" );
	UT_VERIFY( lst[1] == "/usr/share" );

	lst.clear();

	setenv("XDG_CONFIG_DIRS", "/etc/myconf:/etc/yourconf:/usr/local/share/conf:/some/path/conf", 1);
	ret = system_config_dirs(lst);
	UT_VERIFY( ret == 4 );
	UT_VERIFY( lst[0] == "/etc/myconf" );
	UT_VERIFY( lst[1] == "/etc/yourconf" );
	UT_VERIFY( lst[2] == "/usr/local/share/conf" );
	UT_VERIFY( lst[3] == "/some/path/conf" );

	unsetenv("XDG_CONFIG_DIRS");
	lst.clear();

	setenv("XDG_CONFIG_DIRS", "/etc/mydirconf", 1);
	ret = system_config_dirs(lst);
	UT_VERIFY( ret == 1 );
	UT_VERIFY( lst.size() == 1 );
	UT_VERIFY( lst[0] == "/etc/mydirconf" );

	unsetenv("XDG_CONFIG_DIRS");
	lst.clear();

	// in case of empty XDG_CONFIG_DIRS
	ret = system_config_dirs(lst);
	UT_VERIFY( ret == 1 );
	UT_VERIFY( lst[0] == "/etc/xdg" );
}


UT_FUNC(PathBuildTest, "Test path builders")
{
	String s = build_filename("/", "foo", "baz", "taz");
	UT_VERIFY( s == "foo/baz/taz" );

	s = build_filename("/", "/foo/baz////", "baz", "myfile.txt/");
	UT_VERIFY( s == "/foo/baz/baz/myfile.txt" );

	s = build_filename("||", "|foo|", "baz", "file.txt");
	UT_VERIFY( s == "foo||baz||file.txt" );

	s = build_filename("||", "|foo|", "baz", "||||||||file.txt");
	UT_VERIFY( s == "foo||baz||file.txt" );

	s = build_filename("/", "home", "myfile.txt");
	UT_VERIFY( s == "home/myfile.txt" );

	s = build_filename("/", "///home/", "/myfile.txt//");
	UT_VERIFY( s == "/home/myfile.txt" );

	s = build_dirname("/", "/", "home", "mydir");
	UT_VERIFY( s == "/home/mydir/" );

	s = build_dirname("/", "/", "/", "/demodir");
	UT_VERIFY( s == "/demodir/" );

	s = build_dirname("", "/", "/", "/demodir");
	UT_VERIFY( s == "///demodir" );
}
