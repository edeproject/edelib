/*
 * $Id$
 *
 * Various functions
 * Part of edelib.
 * Copyright (c) 2005-2007 EDE Authors.
 *
 * This program is licenced under terms of the 
 * GNU General Public Licence version 2 or newer.
 * See COPYING for details.
 */

#include <edelib/econfig.h>
#include <edelib/Debug.h>
#include <edelib/Util.h>
#include <edelib/StrUtil.h>

#include <stdlib.h> // getenv
#include <string.h> // strlen, strncpy

// default places according to freedesktop
#define CONFIG_HOME_DEFAULT "~/.config/ede"
#define DATA_HOME_DEFAULT   "~/.local/share/ede"
#define CACHE_HOME_DEFAULT  "~/.cache/ede"

#define MAX_PATH 1024

EDELIB_NAMESPACE {

String _config_get(const char* env1, const char* env2, const char* fallback, unsigned int fallback_len)
{
	char* path = getenv(env1);
	if(!path)
		path = getenv(env2);
	// failed env1 and env2; return fallback
	if(!path)
		return fallback;
	/*
	 * getenv() can return empty variable ("") and
	 * will see it as existing; with additional checking
	 * we make sure we have meaningfull data
	 */
	int len = strlen(path);
	if(!len)
		return fallback;

	String ret;
	ret.reserve(MAX_PATH);

	if(path[len-1] == '/') 
		ret.printf("%s%s", path, "ede");
	else
		ret.printf("%s%s", path, "/ede");
	return ret;
}

int _dirs_get(const char* env1, const char* env2, const char* fallback, vector<String>& lst)
{
	EASSERT(fallback != NULL);

	char* path = getenv(env1);
	if(!path)
		path = getenv(env2);
	if(!path)
		path = (char*)fallback;

	stringtok(lst, path, ":");
	return lst.size();
}
#include <stdio.h>
String _path_builder(const char* separator, bool ending, const char* p1, const char* p2 = NULL, const char* p3 = NULL)
{
	bool trailing = false;
	int slen = strlen(separator);

	if(strncmp(p1, separator, slen) == 0)
		trailing = true;

	String str;
	str.reserve(100);
	str = p1;

	if(p2) {
		str += separator;
		str += p2;
	}

	if(p3) {
		str += separator;
		str += p3;
	}

	// separator == ""
	if(!slen)
		return str;

	// now tokenize it
	vector<String> vs;
	stringtok(vs, str, separator);

	// this should not happen
	EASSERT(vs.size() > 0 && "This should not be happened !!!");

	str.clear();
	if(trailing)
		str += separator;

	unsigned int sz = vs.size();
	/*
	 * This is intentionaly so cases like
	 * build_filename("/", "/", "/", "foo") can be accepted
	 */
	if(sz > 1) {
		unsigned int i = 0;
		sz -= 1;
		for(; i < sz; i++) {
			str += vs[i];
			str += separator;
		}
		// take last one
		str += vs[i];
	} else
		str += vs[0];

	if(ending)
		str += separator;

	return str;
}

String user_config_dir(void) 
{ 
	return _config_get("XDG_CONFIG_HOME", "EDE_CONFIG_HOME", CONFIG_HOME_DEFAULT, sizeof(CONFIG_HOME_DEFAULT));
}

String user_data_dir(void) 
{
	return _config_get("XDG_DATA_HOME", "EDE_DATA_HOME", DATA_HOME_DEFAULT, sizeof(DATA_HOME_DEFAULT));
}

String user_cache_dir(void) 
{
	return _config_get("XDG_CACHE_HOME", "EDE_CACHE_HOME", CACHE_HOME_DEFAULT, sizeof(CACHE_HOME_DEFAULT));
}

int system_config_dirs(vector<String>& lst)
{
	/*
	 * stringtok will correctly see /etc/xdg (without ':')
	 * as one entry
	 */
	return _dirs_get("XDG_CONFIG_DIRS", "EDE_CONFIG_DIRS", "/etc/xdg", lst);
}

int system_data_dirs(vector<String>& lst)
{
	return _dirs_get("XDG_DATA_DIRS", "EDE_DATA_DIRS", "/usr/local/share:/usr/share", lst);
}

String build_filename(const char* separator, const char* p1, const char* p2, const char* p3)
{
	EASSERT(separator != NULL);
	EASSERT(p1 != NULL);

	return _path_builder(separator, false, p1, p2, p3);
}

String build_dirname(const char* separator, const char* p1, const char* p2, const char* p3)
{
	EASSERT(separator != NULL);
	EASSERT(p1 != NULL);

	return _path_builder(separator, true, p1, p2, p3);
}

}
