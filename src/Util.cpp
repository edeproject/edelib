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
#include <stdio.h>  // snprintf

// default places according to freedesktop
#define CONFIG_HOME_DEFAULT "~/.config/ede/"
#define DATA_HOME_DEFAULT   "~/.local/share/ede/"
#define CACHE_HOME_DEFAULT  "~/.cache/ede/"

#define MAX_PATH 1024

EDELIB_NAMESPACE {

// TODO: this buffer should be locked
static char path_buff[MAX_PATH];

const char* _config_get(const char* env1, const char* env2, const char* fallback, unsigned int fallback_len)
{
	char* path = getenv(env1);
	if(!path)
		path = getenv(env2);

	// TODO: what about plain == "", since is valid environ variable ?
	if(path)
	{
		int len = strlen(path);
		if(path[len-1] == '/') 
			snprintf(path_buff, MAX_PATH, "%s%s", path, "ede/");
		else
			snprintf(path_buff, MAX_PATH, "%s%s", path, "/ede/");
	}
	else
		strncpy(path_buff, fallback, fallback_len);
	return path_buff;
}

const char* user_config_dir(void) 
{ 
	return _config_get("XDG_CONFIG_HOME", "EDE_CONFIG_HOME", CONFIG_HOME_DEFAULT, sizeof(CONFIG_HOME_DEFAULT));
}

const char* user_data_dir(void) 
{
	return _config_get("XDG_DATA_HOME", "EDE_DATA_HOME", DATA_HOME_DEFAULT, sizeof(DATA_HOME_DEFAULT));
}

const char* user_cache_dir(void) 
{
	return _config_get("XDG_CACHE_HOME", "EDE_CACHE_HOME", CACHE_HOME_DEFAULT, sizeof(CACHE_HOME_DEFAULT));
}

}
