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

#include <edelib/Debug.h>
#include <edelib/Util.h>
#include <edelib/StrUtil.h>
#include <edelib/Directory.h>

#include <stdlib.h> // getenv
#include <string.h> // strlen

//#define MAX_PATH 1024

EDELIB_NS_BEGIN

typedef list<String> StringList;
typedef list<String>::iterator StringListIter;

static String _config_get(const char* env, const char* fallback) {
	int len = 0;
	char* path = getenv(env);
	if(!path)
		goto bail;
	/*
	 * getenv() can return empty variable ("") and
	 * will see it as existing; with additional checking
	 * we make sure we have meaningfull data
	 */
	len = strlen(path);
	if(!len)
		goto bail;

	// case when we got only one character in path
	if(len == 1)
		return path;
	else {
		String ret;
		ret.reserve(len);
		// assure path does not ends with slash.
		if(path[len-1] == '/') 
			ret.assign(path, len-1);
		else
			ret.assign(path);
		return ret;
	}

bail:
	String ret;
	ret.reserve(128);

	ret = dir_home();
	// unable to fetch HOME @#??#; fall to '~/'
	if(ret.empty())
		ret = "~/";

	ret += fallback;
	return ret;
}

static int _dirs_get(const char* env, const char* fallback, StringList& lst) {
	EASSERT(fallback != NULL);

	char* path = getenv(env);
	if(!path)
		path = (char*)fallback;

	stringtok(lst, path, ":");
	return lst.size();
}

static String _path_builder(const char* separator, bool ending, const char* p1, const char* p2 = NULL, const char* p3 = NULL) {
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
	StringList ls;
	stringtok(ls, str, separator);

	EASSERT(ls.size() > 0 && "This should not be happened !!!");

	str.clear();
	if(trailing)
		str += separator;

	unsigned int sz = ls.size();
	StringListIter it = ls.begin();

	/*
	 * This is intentionaly so cases like
	 * build_filename("/", "/", "/", "foo") can be accepted
	 */
	if(sz > 1) {
		sz -= 1;
		for(; sz; sz--) {
			str += (*it);
			str += separator;
			++it;
		}
		// take last one
		str += (*it);
	} else {
		// then just take first one
		str += (*it);
	}

	if(ending)
		str += separator;

	return str;
}

String user_config_dir(void) { 
	return _config_get("XDG_CONFIG_HOME", "/.config");
}

String user_data_dir(void) {
	return _config_get("XDG_DATA_HOME", "/.local/share");
}

String user_cache_dir(void) {
	return _config_get("XDG_CACHE_HOME", "/.cache");
}

int system_config_dirs(StringList& lst) {
	/*
	 * stringtok will correctly see /etc/xdg (without ':')
	 * as one entry
	 */
	return _dirs_get("XDG_CONFIG_DIRS", "/etc/xdg", lst);
}

int system_data_dirs(StringList& lst) {
	return _dirs_get("XDG_DATA_DIRS", "/usr/local/share:/usr/share", lst);
}

String build_filename(const char* separator, const char* p1, const char* p2, const char* p3) {
	EASSERT(separator != NULL);
	EASSERT(p1 != NULL);

	return _path_builder(separator, false, p1, p2, p3);
}

String build_dirname(const char* separator, const char* p1, const char* p2, const char* p3) {
	EASSERT(separator != NULL);
	EASSERT(p1 != NULL);

	return _path_builder(separator, true, p1, p2, p3);
}

EDELIB_NS_END
