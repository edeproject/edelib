#include <edelib/Resource.h>
#include <edelib/Directory.h>
#include <edelib/File.h>
#include <edelib/Missing.h>
#include <stdlib.h>
#include <string.h>
#include "UnitTest.h"

EDELIB_NS_USE

#define CCHARP(str)           ((const char*)str)
#define STR_EQUAL(str1, str2) (strcmp(CCHARP(str1), CCHARP(str2)) == 0)

#define SAMPLE_RES_DIR  "res-data"
#define SAMPLE_SYS_DIR   SAMPLE_RES_DIR"/sys"
#define SAMPLE_USER_DIR  SAMPLE_RES_DIR"/user"

#define CONFIG_HOME_ENV "XDG_CONFIG_HOME"
#define CONFIG_DIRS_ENV "XDG_CONFIG_DIRS"

UT_FUNC(ResourceTestReading, "Test resource reading")
{
	dir_create(SAMPLE_RES_DIR);
	dir_create(SAMPLE_SYS_DIR);
	dir_create(SAMPLE_USER_DIR);

	const char* sys_conf = "\
[global]          \n\
int_value = 3     \n\
char_value = c    \n\
float_value = 0.1 \n\
str_value = this is sample string value\n\
";

	const char* user_conf = "\
[global]              \n\
float_value = 4.99    \n\
int_value = 45        \n\
unique_char_value = u \n\
str_value = this is sample string value from user\n\
long_str_value = aaaaaaaaaaaaaaaaaaa aaaaaaaaaaaaaaaaaaa aaaaaaaaaaaaaa aaaaaaaaaaaaa\n\
";

	File f;
	f.open(SAMPLE_SYS_DIR"/foo.conf", FIO_WRITE);
	f.write(sys_conf);
	f.close();

	f.open(SAMPLE_USER_DIR"/foo.conf", FIO_WRITE);
	f.write(user_conf);
	f.close();

	// save previous values
	char* p;
	String config_home_saved;
	String config_dirs_saved;

	p = getenv(CONFIG_HOME_ENV);
	if(p)
		config_home_saved = p;
	p = getenv(CONFIG_DIRS_ENV);
	if(p)
		config_dirs_saved = p;

	edelib_setenv(CONFIG_HOME_ENV, SAMPLE_USER_DIR, 1);
	edelib_setenv(CONFIG_DIRS_ENV, SAMPLE_SYS_DIR, 1);

	// now read from them
	Resource r;
	UT_VERIFY( r.load("foo") == true );
	UT_VERIFY( r == true );

	int   int_ret;
	float float_ret;
	char  char_ret;
	char  buff[128];

	UT_VERIFY( r.get("global", "int_value", int_ret, 0) == true );
	UT_VERIFY( int_ret == 45 ); // user config is prefered

	UT_VERIFY( r.get("global", "float_value", float_ret, 0) == true );
	// uh... these floats...
	UT_VERIFY( float_ret <= 4.99 && float_ret >= 4.98 ); // user config is prefered

	// not in user config
	UT_VERIFY( r.get("global", "char_value", char_ret, 0) == true );
	UT_VERIFY( char_ret == 'c' );

	// not existing
	UT_VERIFY( r.get("global-not-existing", "char_value", char_ret, 0) == false );
	UT_VERIFY( char_ret == 0 );

	// sys config only
	UT_VERIFY( r.get("global", "int_value", int_ret, 0, RES_SYS_ONLY) == true );
	UT_VERIFY( int_ret == 3 );

	// sys config only
	UT_VERIFY( r.get("global", "float_value", float_ret, 0, RES_SYS_ONLY) == true );
	UT_VERIFY( float_ret >= 0.1 && float_ret <= 0.1001 ); 

	// from user config (default)
	UT_VERIFY( r.get("global", "str_value", buff, 128) == true );
	UT_VERIFY( STR_EQUAL(buff, "this is sample string value from user") == true );

	// from sys config only
	UT_VERIFY( r.get("global", "str_value", buff, 128, RES_SYS_ONLY) == true );
	UT_VERIFY( STR_EQUAL(buff, "this is sample string value") == true );

	char* alloc_buff;
	unsigned int retsz;
	UT_VERIFY( r.get_allocated("global", "long_str_value", &alloc_buff, retsz) == true );
	UT_VERIFY( STR_EQUAL(alloc_buff, "aaaaaaaaaaaaaaaaaaa aaaaaaaaaaaaaaaaaaa aaaaaaaaaaaaaa aaaaaaaaaaaaa") == true );
	delete [] alloc_buff;

	// if fails, returned size must be set to 0
	UT_VERIFY( r.get_allocated("global", "not-exists", &alloc_buff, retsz) == false );
	UT_VERIFY( retsz == 0 );

	// restore env vars and delete files
	edelib_setenv(CONFIG_HOME_ENV, config_home_saved.c_str(), 1);
	edelib_setenv(CONFIG_DIRS_ENV, config_dirs_saved.c_str(), 1);

	file_remove(SAMPLE_USER_DIR"/foo.conf");
	file_remove(SAMPLE_SYS_DIR"/foo.conf");
	dir_remove(SAMPLE_SYS_DIR);
	dir_remove(SAMPLE_USER_DIR);
	dir_remove(SAMPLE_RES_DIR);

	// test failings
	r.clear();
	UT_VERIFY( r == false );

	UT_VERIFY( r.load("this-one-does-not-exists") == false );
	UT_VERIFY( r == false );
}

UT_FUNC(ResourceTestSaving, "Test resource saving")
{
	dir_create(SAMPLE_RES_DIR);
	dir_create(SAMPLE_SYS_DIR);
	dir_create(SAMPLE_USER_DIR);

	const char* sys_conf = "\
[global]          \n\
int_value = 3     \n\
";

	const char* user_conf = "\
[global]              \n\
int_value = 4         \n\
user_altered_value = 5\n\
";

	File f;
	f.open(SAMPLE_SYS_DIR"/foo.conf", FIO_WRITE);
	f.write(sys_conf);
	f.close();

	f.open(SAMPLE_USER_DIR"/foo.conf", FIO_WRITE);
	f.write(user_conf);
	f.close();

	// save previous values
	char* p;
	String config_home_saved;
	String config_dirs_saved;

	p = getenv(CONFIG_HOME_ENV);
	if(p)
		config_home_saved = p;
	p = getenv(CONFIG_DIRS_ENV);
	if(p)
		config_dirs_saved = p;

	edelib_setenv(CONFIG_HOME_ENV, SAMPLE_USER_DIR, 1);
	edelib_setenv(CONFIG_DIRS_ENV, SAMPLE_SYS_DIR, 1);

	// now read from them
	Resource r;
	r.load("foo");
	UT_VERIFY( r == true );

	r.set("global", "int_value", 6);
	UT_VERIFY( r.save("foo") == true );

	r.load("foo");
	UT_VERIFY( r == true );

	// user altered value must not be truncated
	int ret;
	UT_VERIFY( r.get("global", "user_altered_value", ret, 0) == true );
	UT_VERIFY( ret == 5 );

	// restore env vars and delete files
	edelib_setenv(CONFIG_HOME_ENV, config_home_saved.c_str(), 1);
	edelib_setenv(CONFIG_DIRS_ENV, config_dirs_saved.c_str(), 1);

	file_remove(SAMPLE_USER_DIR"/foo.conf");
	file_remove(SAMPLE_SYS_DIR"/foo.conf");
	dir_remove(SAMPLE_SYS_DIR);
	dir_remove(SAMPLE_USER_DIR);
	dir_remove(SAMPLE_RES_DIR);
}

UT_FUNC(ResourceTestBuggy, "Test resource buggy read/save")
{
	dir_create(SAMPLE_RES_DIR);
	dir_create(SAMPLE_SYS_DIR);
	dir_create(SAMPLE_USER_DIR);

	const char* sys_conf = "\
[global]          \n\
int_value = 3     \n\
buggy_value = 3   \n\
";

	const char* user_conf = "\
[global]              \n\
int_value = 4         \n\
user_altered_value = 5\n\
buggy buggy value = 3 \n\
";

	File f;
	f.open(SAMPLE_SYS_DIR"/foo-not-found.conf", FIO_WRITE);
	f.write(sys_conf);
	f.close();

	f.open(SAMPLE_USER_DIR"/foo.conf", FIO_WRITE);
	f.write(user_conf);
	f.close();

	// save previous values
	char* p;
	String config_home_saved;
	String config_dirs_saved;

	p = getenv(CONFIG_HOME_ENV);
	if(p)
		config_home_saved = p;
	p = getenv(CONFIG_DIRS_ENV);
	if(p)
		config_dirs_saved = p;

	edelib_setenv(CONFIG_HOME_ENV, SAMPLE_USER_DIR, 1);
	edelib_setenv(CONFIG_DIRS_ENV, SAMPLE_SYS_DIR, 1);

	// now read from them
	Resource r;
	r.load("foo");
	UT_VERIFY( r == true );

	r.set("global", "int_value", 6);
	UT_VERIFY( r.save("foo") == true );

	r.load("foo");
	UT_VERIFY( r == true );

	// user altered value must not be truncated
	int ret;
	UT_VERIFY( r.get("global", "user_altered_value", ret, 0) == true );
	UT_VERIFY( ret == 5 );

	// should be 'buggy_value'
	UT_VERIFY( r.get("global", "buggy value", ret, 3, RES_SYS_FIRST) == false );

	// shoud be 'buggy buggy value'
	UT_VERIFY( r.get("global", "buggy_buggy_value", ret, 3) == false );

	// restore env vars and delete files
	edelib_setenv(CONFIG_HOME_ENV, config_home_saved.c_str(), 1);
	edelib_setenv(CONFIG_DIRS_ENV, config_dirs_saved.c_str(), 1);

	file_remove(SAMPLE_USER_DIR"/foo.conf");
	file_remove(SAMPLE_SYS_DIR"/foo-not-found.conf");
	dir_remove(SAMPLE_SYS_DIR);
	dir_remove(SAMPLE_USER_DIR);
	dir_remove(SAMPLE_RES_DIR);
}

UT_FUNC(ResourceTestFindConf, "Test resource config find")
{
	dir_create(SAMPLE_RES_DIR);
	dir_create(SAMPLE_SYS_DIR);
	dir_create(SAMPLE_USER_DIR);

	const char* sys_conf = "\
[global]          \n\
int_value = 3     \n\
";

	const char* user_conf = "\
[global]              \n\
int_value = 4         \n\
user_altered_value = 5\n\
";

	File f;
	f.open(SAMPLE_SYS_DIR"/foo.conf", FIO_WRITE);
	f.write(sys_conf);
	f.close();

	f.open(SAMPLE_USER_DIR"/foo.conf", FIO_WRITE);
	f.write(user_conf);
	f.close();

	// save previous values
	char* p;
	String config_home_saved;
	String config_dirs_saved;

	p = getenv(CONFIG_HOME_ENV);
	if(p)
		config_home_saved = p;
	p = getenv(CONFIG_DIRS_ENV);
	if(p)
		config_dirs_saved = p;

	edelib_setenv(CONFIG_HOME_ENV, SAMPLE_USER_DIR, 1);
	edelib_setenv(CONFIG_DIRS_ENV, SAMPLE_SYS_DIR, 1);

	// check it's location
	UT_VERIFY( Resource::find_config("foo") == SAMPLE_USER_DIR"/foo.conf");
	UT_VERIFY( Resource::find_config("foo", RES_USER_FIRST) == SAMPLE_USER_DIR"/foo.conf");
	UT_VERIFY( Resource::find_config("foo", RES_USER_ONLY) == SAMPLE_USER_DIR"/foo.conf");

	UT_VERIFY( Resource::find_config("foo", RES_SYS_FIRST) == SAMPLE_SYS_DIR"/foo.conf");
	UT_VERIFY( Resource::find_config("foo", RES_SYS_ONLY) == SAMPLE_SYS_DIR"/foo.conf");

	// restore env vars and delete files
	edelib_setenv(CONFIG_HOME_ENV, config_home_saved.c_str(), 1);
	edelib_setenv(CONFIG_DIRS_ENV, config_dirs_saved.c_str(), 1);

	file_remove(SAMPLE_USER_DIR"/foo.conf");
	file_remove(SAMPLE_SYS_DIR"/foo.conf");
	dir_remove(SAMPLE_SYS_DIR);
	dir_remove(SAMPLE_USER_DIR);
	dir_remove(SAMPLE_RES_DIR);
}


UT_FUNC(ResourceTestFindDir, "Test resource dir find")
{
	dir_create(SAMPLE_RES_DIR);
	dir_create(SAMPLE_SYS_DIR);
	dir_create(SAMPLE_USER_DIR);

	dir_create(SAMPLE_USER_DIR"/baz");
	dir_create(SAMPLE_SYS_DIR"/taz");

	// save previous values
	char* p;
	String config_home_saved;
	String config_dirs_saved;

	p = getenv(CONFIG_HOME_ENV);
	if(p)
		config_home_saved = p;
	p = getenv(CONFIG_DIRS_ENV);
	if(p)
		config_dirs_saved = p;

	edelib_setenv(CONFIG_HOME_ENV, SAMPLE_USER_DIR, 1);
	edelib_setenv(CONFIG_DIRS_ENV, SAMPLE_SYS_DIR, 1);

	// check it's location
	UT_VERIFY( Resource::find_dir("baz") == SAMPLE_USER_DIR"/baz");
	UT_VERIFY( Resource::find_dir("baz", RES_USER_FIRST) == SAMPLE_USER_DIR"/baz");
	UT_VERIFY( Resource::find_dir("baz", RES_USER_ONLY) == SAMPLE_USER_DIR"/baz");

	UT_VERIFY( Resource::find_dir("taz", RES_SYS_FIRST) == SAMPLE_SYS_DIR"/taz");
	UT_VERIFY( Resource::find_dir("taz", RES_SYS_ONLY) == SAMPLE_SYS_DIR"/taz");

	// restore env vars and delete files
	edelib_setenv(CONFIG_HOME_ENV, config_home_saved.c_str(), 1);
	edelib_setenv(CONFIG_DIRS_ENV, config_dirs_saved.c_str(), 1);

	dir_remove(SAMPLE_USER_DIR"/baz");
	dir_remove(SAMPLE_SYS_DIR"/taz");

	dir_remove(SAMPLE_SYS_DIR);
	dir_remove(SAMPLE_USER_DIR);
	dir_remove(SAMPLE_RES_DIR);
}
