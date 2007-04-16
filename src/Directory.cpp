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
#include <edelib/StrUtil.h>

#include <stdlib.h>    // getenv
#include <sys/types.h> // stat
#include <sys/stat.h>  // stat
#include <unistd.h>    // access, stat, getcwd
#include <dirent.h>    // scandir, dirent

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

bool dir_list(const char* dir, vector<String>& lst, bool full_path, bool show_hidden)
{
	if(!dir_readable(dir))
		return false;

	dirent** files;
	int n = scandir(dir, &files, NULL, alphasort);
	if(n < 0)
		return false;

	lst.clear();
	lst.reserve(n);

	bool have_sep;
	if(str_ends(dir, dir_separator().c_str()))
		have_sep = true;
	else
		have_sep = false;

	// fill our vector and clean in the same time
	for(int i = 0; i < n; i++) 
	{
		if(!show_hidden && files[i]->d_name[0] == '.')
		{
			free(files[i]);
			continue;
		}

		if(full_path)
		{
			String tmp = dir;
			if(!have_sep)
				tmp += dir_separator();
			tmp += files[i]->d_name;
			lst.push_back(tmp);
		}
		else
			lst.push_back(files[i]->d_name);

		free(files[i]);
	}

	free(files);
	return true;
}

}
