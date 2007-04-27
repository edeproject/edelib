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

}
