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
#include <edelib/File.h>

#include <stdlib.h>    // getenv
#include <sys/types.h> // stat, struct passwd, mkdir, opendir, closedir,readdir
#include <sys/stat.h>  // stat, mkdir
#include <unistd.h>    // access, stat, getcwd, getuid, sysconf, rmdir
#include <dirent.h>    // scandir, dirent, opendir, closedir,readdir
#include <pwd.h>       // getpwuid_r
#include <stdio.h>     // rename
#include <errno.h>
#include <stdio.h>     // snprintf

#ifndef PATH_MAX
	#define PATH_MAX 256
#endif

#define DOT_OR_DOTDOT(base) \
	(base[0] == '.' && (base[1] == '\0' || base[1] == '.' && base[2] == '\0'))

EDELIB_NS_BEGIN

#if 0
bool dirwalk(const char* dir, 
		void (*on_dir_enter)(const char*, const char*), void (*on_dir_leave)(const char*, const char*), 
		void (*on_file)(const char*, const char*), 
		const char* data, void (*progress)(const char*)) {

	char name[PATH_MAX];
	struct dirent* dp;
	DIR* dfd;
	int dirlen = 0;

	if((dfd = opendir(dir)) == NULL) {
		EDEBUG(ESTRLOC ": Can't open %s\n", dir);
		return false;
	}

	dirlen = strlen(dir);

	while((dp = readdir(dfd)) != NULL) {
		if(DOT_OR_DOTDOT(dp->d_name))
			continue;

		if(dirlen + strlen(dp->d_name) + 2 > sizeof(name))
			EDEBUG(ESTRLOC ": %s too long\n", dp->d_name);
		else {
			if(dirlen > 0 && dir[dirlen-1] == '/')
				snprintf(name, sizeof(name), "%s%s", dir, dp->d_name);
			else
				snprintf(name, sizeof(name), "%s/%s", dir, dp->d_name);

			if(progress)
				progress(name);

			if(dir_exists(name)) {
				//printf("entering in %s\n", name);

				if(on_dir_enter)
					on_dir_enter(name, data);

				dirwalk(name, on_dir_enter, on_dir_leave, on_file, data, progress);

				//printf("leaving %s\n", name);
				if(on_dir_leave)
					on_dir_leave(name, data);
			} else {
				// FIXME: for now everything else is seen as file
				on_file(name, data);
			}
		}
	}

	closedir(dfd);
	return true;
}
#endif

void d_on_enter(const char* name, const char*) {
	EDEBUG(ESTRLOC ": on_enter_dir : %s\n", name);
}

void d_on_leave(const char* name, const char*) {
	EDEBUG(ESTRLOC ": on_leave_dir : %s\n", name);
}

void d_on_file(const char* name, const char* d) {
	EDEBUG(ESTRLOC ": on_file : %s -> %s%s\n", name, d, name);
}

bool dir_exists(const char* name) {
	EASSERT(name != NULL);
	struct stat s;
	if(stat(name, &s) != 0)
		return false;
	return (access(name, F_OK) == 0) && S_ISDIR(s.st_mode);
}

bool dir_readable(const char* name) {
	EASSERT(name != NULL);
	struct stat s;
	if(stat(name, &s) != 0)
		return false;
	return (access(name, R_OK) == 0) && S_ISDIR(s.st_mode);
}

bool dir_writeable(const char* name) {
	EASSERT(name != NULL);
	struct stat s;
	if(stat(name, &s) != 0)
		return false;
	return (access(name, W_OK) == 0) && S_ISDIR(s.st_mode);
}

bool dir_create(const char* name, int perm) {
	EASSERT(name != NULL);

	/*
	 * Checks are not done (line in dir_remove() case since
	 * mkdir() can fail if any system object with given path/name
	 * exists. We will rely here on mkdir() return codes.
	 */
	int ret = mkdir(name, perm);
	if(ret == 0)
		return true;

	return false;
}

bool dir_remove(const char* name) {
	EASSERT(name != NULL);

	if(dir_exists(name)) {
		int ret = rmdir(name);
		if(ret == 0)
			return true;
		else
			return false;
	}

	return false;
}

bool dir_remove_rec(const char* dir, bool all, void (*progress)(const char* name), void (*on_fail)(const char* name)) {
	EASSERT(dir != NULL);

	char name[PATH_MAX];
	struct dirent* dp;
	DIR* dfd;
	int dirlen = 0;

	if((dfd = opendir(dir)) == NULL) {
		if(on_fail)
			on_fail(dir);
		return false;
	}

	dirlen = strlen(dir);

	while((dp = readdir(dfd)) != NULL) {
		if(DOT_OR_DOTDOT(dp->d_name))
			continue;

		if(dirlen + strlen(dp->d_name) + 2 > sizeof(name)) {
			EDEBUG(ESTRLOC ": %s/%s too long\n", dir, dp->d_name);

			if(on_fail)
				on_fail(name);

		} else {
			if(dirlen > 0 && dir[dirlen-1] == '/')
				snprintf(name, sizeof(name), "%s%s", dir, dp->d_name);
			else
				snprintf(name, sizeof(name), "%s/%s", dir, dp->d_name);

			if(progress)
				progress(name);

			if(dir_exists(name)) {
				//printf("entering in %s\n", name);
				dir_remove_rec(name, all, progress, on_fail);
				//printf("leaving %s\n", name);

				if(all) {
					EDEBUG("-> rmdir %s\n", name);
					if(!dir_remove(name))
						EDEBUG("Can't delete %s directory\n", name);
				}
			} else {
				// FIXME: for now everything else is seen as file
				EDEBUG("-> rm    %s\n", name);

				if(!file_remove(name))
					EDEBUG("Can't delete %s\n", name);
			}
		}
	}

	closedir(dfd);
	return true;
}

bool dir_rename(const char* from, const char* to) {
	EASSERT(from != NULL);
	EASSERT(to != NULL);

	if(!dir_exists(from))
		return false;

	int ret = rename(from, to);
	if(ret == 0)
		return true;
	return false;
}

// stolen from coreutils (cp,mv) package
static struct dirent* readdir_ignoring_dots(DIR* dirp) {
	struct dirent* dp = NULL;

	while(1) {
		dp = readdir(dirp);
		if(dp == NULL || !DOT_OR_DOTDOT(dp->d_name))
			break;
	}

	return dp;
}

bool dir_empty(const char* name) {
	EASSERT(name != NULL);

	if(!dir_exists(name))
		return false;

	DIR* dirp = opendir(name);
	if(dirp == NULL)
		return false;

	errno = 0;
	struct dirent* dp = readdir_ignoring_dots(dirp);
	int saved_errno = errno;

	closedir(dirp);
	if(dp != NULL)
		return false;

	return (saved_errno == 0 ? true : false);
}

String dir_home(void) {
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
	 * This looks like a leak in glibc.
	 */
	long buffsz = sysconf(_SC_GETPW_R_SIZE_MAX);
	if(buffsz < 0)
		buffsz = 1024;

	struct passwd pwd_str;
	struct passwd* pw = NULL;

	char* buff = new char[buffsz];
	int err = getpwuid_r(getuid(), &pwd_str, buff, buffsz, &pw);

	if(err) {
		delete [] buff;
		return "";
	}

	EASSERT(pw != NULL);

	String ret = pw->pw_dir;
	delete [] buff;

	return ret;
}

String dir_current(void) {
	char buff[PATH_MAX];
	if(getcwd(buff, PATH_MAX))
		return buff;
	else
		return "";
}

const char* dir_separator(void) {
	return "/";
}

bool dir_list(const char* dir, list<String>& lst, bool full_path, bool show_hidden, bool clear) {
	if(!dir_readable(dir))
		return false;

	String dirstr = dir;
	if(dirstr == ".")
		dirstr.assign(dir_current());

	dirent** files;
	int n = scandir(dirstr.c_str(), &files, NULL, alphasort);
	if(n < 0)
		return false;

	if(clear)
		lst.clear();

	bool have_sep;
	if(str_ends(dirstr.c_str(), dir_separator()))
		have_sep = true;
	else
		have_sep = false;

	String tmp;
	tmp.reserve(PATH_MAX);
	
	// fill our list and clean in the same time
	for(int i = 0; i < n; i++) {
		if(!show_hidden && files[i]->d_name[0] == '.') {
			free(files[i]);
			continue;
		}

		if(full_path) {
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

EDELIB_NS_END
