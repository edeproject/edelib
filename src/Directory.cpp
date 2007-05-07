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
#include <sys/types.h> // stat, struct passwd
#include <sys/stat.h>  // stat
#include <unistd.h>    // access, stat, getcwd, getuid, sysconf
#include <dirent.h>    // scandir, dirent
#include <pwd.h>       // getpwuid_r

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

	/*
	 * Fallback, read passwd structure, firstly acquiring it with
	 * _SC_GETPW_R_SIZE_MAX, then with our own size.
	 * 
	 * Note that valgrind will report leak here, with this message:
	 *   __nss_database_lookup (in /lib/tls/libc-2.3.5.so)
	 *   getpwuid_r@@GLIBC_2.1.2 (in /lib/tls/libc-2.3.5.so)
	 * This looks like leak in glibc.
	 */
	long buffsz = sysconf(_SC_GETPW_R_SIZE_MAX);
	if(buffsz < 0)
		buffsz = 1024;

	struct passwd pwd_str;
	struct passwd* pw = NULL;

	char* buff = new char[buffsz];
	int err = getpwuid_r(getuid(), &pwd_str, buff, buffsz, &pw);

	if(err)
	{
		delete [] buff;
		return "";
	}

	EASSERT(pw != NULL);

	String ret = pw->pw_dir;
	delete [] buff;

	return ret;
}

String dir_current(void)
{
	char buff[PATH_MAX];
	if(getcwd(buff, PATH_MAX))
		return buff;
	else
		return "";
}

const char* dir_separator(void)
{
	return "/";
}

bool dir_list(const char* dir, vector<String>& lst, bool full_path, bool show_hidden)
{
	if(!dir_readable(dir))
		return false;

	String dirstr = dir;
	if(dirstr == ".")
		dirstr.assign(dir_current());

	dirent** files;
	int n = scandir(dirstr.c_str(), &files, NULL, alphasort);
	if(n < 0)
		return false;

	lst.clear();
	lst.reserve(n);

	bool have_sep;
	if(str_ends(dirstr.c_str(), dir_separator()))
		have_sep = true;
	else
		have_sep = false;

	String tmp;
	tmp.reserve(PATH_MAX);
	
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
			tmp = dirstr;

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
