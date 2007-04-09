/*
 * $Id$
 *
 * Directory handlers
 * Part of edelib.
 * Copyright (c) 2005-2007 EDE Authors.
 *
 * This program is licenced under terms of the 
 * GNU General Public Licence version 2 or newer.
 * See COPYING for details.
 */

#include <edelib/econfig.h>
#include <edelib/Directory.h>
#include <edelib/Debug.h>

#include <stdlib.h> // getenv
#include <unistd.h> // getcwd

#include <sys/types.h> // stat
#include <sys/stat.h>  // stat
#include <unistd.h>    // access, stat

#ifndef PATH_MAX
	#define PATH_MAX 256
#endif

EDELIB_NAMESPACE {

bool dir_exists(const char* name)
{
	EASSERT(name != NULL);
	struct stat s;
	if(stat(name, &s) != 0)
		return false;
	return (access(name, F_OK) == 0) && S_ISDIR(s.st_mode);
}

bool dir_readable(const char* name)
{
	EASSERT(name != NULL);
	struct stat s;
	if(stat(name, &s) != 0)
		return false;
	return (access(name, R_OK) == 0) && S_ISDIR(s.st_mode);
}

bool dir_writeable(const char* name)
{
	EASSERT(name != NULL);
	struct stat s;
	if(stat(name, &s) != 0)
		return false;
	return (access(name, W_OK) == 0) && S_ISDIR(s.st_mode);
}

String dir_home(void)
{
	char* p = getenv("HOME");
	if(p)
		return p;
	else
		return "";
}

String dir_current(void)
{
	char buff[PATH_MAX];
	if(getcwd(buff, PATH_MAX))
		return buff;
	else
		return "";
}

String dir_separator(void)
{
	return "/";
}

}
