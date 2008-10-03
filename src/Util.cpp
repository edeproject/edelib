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

#include <stdlib.h> // getenv
#include <string.h> // strlen

#include <edelib/Debug.h>
#include <edelib/Util.h>
#include <edelib/StrUtil.h>
#include <edelib/Directory.h>

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
	E_ASSERT(fallback != NULL);

	char* path = getenv(env);
	if(!path)
		path = (char*)fallback;

	stringtok(lst, path, ":");
	return lst.size();
}

static void _build_path_segment(const char* separator, int separator_len, 
		const char* path, 
		int keep_leading, int keep_trailing, 
		String& ret) 
{
	const char* p = path;
	const char* end;

	if(separator_len && !keep_leading) {
		while(p && strncmp(p, separator, separator_len) == 0)
			p += separator_len;
	}

	end = p + strlen(p);

	if(separator_len && !keep_trailing) {
		const char* t = p + separator_len;

		while(end >= t && strncmp(end - separator_len, separator, separator_len) == 0)
			end -= separator_len;
	}

	ret.append(p, end - p);
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

String build_filename(const char* p1, const char* p2, const char* p3) {
	E_ASSERT(p1 != NULL);
	
	String ret;
	int separator_len = strlen(E_DIR_SEPARATOR_STR);
	int keep_trailing = 1;

	// see if we have other params so can keep trailing separator
	if(!p2 && !p3)
		keep_trailing = 1;
	else
		keep_trailing = 0;

	_build_path_segment(E_DIR_SEPARATOR_STR, separator_len, p1, 1, keep_trailing, ret);

	if(p2) {
		ret += E_DIR_SEPARATOR_STR;

		if(p3)
			keep_trailing = 0;
		else
			keep_trailing = 1;

		_build_path_segment(E_DIR_SEPARATOR_STR, separator_len, p2, 0, keep_trailing, ret);
	}

	if(p3) {
		ret += E_DIR_SEPARATOR_STR;
		_build_path_segment(E_DIR_SEPARATOR_STR, separator_len, p3, 0, 1, ret);
	}

	return ret;
}

EDELIB_NS_END
