/*
 * Directory handlers
 * Copyright (c) 2005-2007 edelib authors
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>    // getenv
#include <sys/types.h> // stat, struct passwd, mkdir, opendir, closedir,readdir
#include <sys/stat.h>  // stat, mkdir
#include <unistd.h>    // access, stat, getcwd, getuid, sysconf, rmdir
#include <dirent.h>    // scandir, dirent, opendir, closedir,readdir
#include <pwd.h>       // getpwuid_r
#include <stdio.h>     // rename
#include <errno.h>
#include <stdio.h>     // snprintf
#include <string.h>

#include <edelib/Directory.h>
#include <edelib/Debug.h>
#include <edelib/StrUtil.h>
#include <edelib/File.h>
#include <edelib/FileTest.h>

#ifndef PATH_MAX
#define PATH_MAX 256
#endif

#define DOT_OR_DOTDOT(base) (base[0] == '.' && (base[1] == '\0' || (base[1] == '.' && base[2] == '\0')))

EDELIB_NS_BEGIN

bool dir_exists(const char* name) {
	E_ASSERT(name != NULL);
	struct stat s;

	E_RETURN_VAL_IF_FAIL(stat(name, &s) == 0, false);
	return (access(name, F_OK) == 0) && S_ISDIR(s.st_mode);
}

bool dir_readable(const char* name) {
	E_ASSERT(name != NULL);
	struct stat s;

	E_RETURN_VAL_IF_FAIL(stat(name, &s) == 0, false);
	return (access(name, R_OK) == 0) && S_ISDIR(s.st_mode);
}

bool dir_writeable(const char* name) {
	E_ASSERT(name != NULL);
	struct stat s;

	E_RETURN_VAL_IF_FAIL(stat(name, &s) == 0, false);
	return (access(name, W_OK) == 0) && S_ISDIR(s.st_mode);
}

bool dir_create(const char* name, int perm) {
	E_ASSERT(name != NULL);
	return (mkdir(name, perm) == 0);
}

/*
 * Largely stolen from GLib (kudos to the GLib team).
 *
 * This function can be simplified via strtok_r() or stringtok() where
 * tokenizers will normalize cases like "/foo///baz///foo". On other
 * hand, above path is valid to every mkdir() so this is not an issue
 * too much.
 */
bool dir_create_with_parents(const char* name, int perm) {
	E_ASSERT(name != NULL);

	char* fn = strdup(name);
	char* p = fn;

	// skip root if needed
	if(fn[0] == E_DIR_SEPARATOR) {
		p = fn;
		while(*p == E_DIR_SEPARATOR)
			p++;
	} else
		p = fn;

	do {
		while(*p && *p != E_DIR_SEPARATOR)
			p++;
		if(!*p)
			p = NULL;
		else
			*p = '\0';

		/* FIXME: file_test() is needed here; can it be removed? */
		if(!file_test(fn, FILE_TEST_IS_DIR) && !dir_create(fn, perm)) {
			free(fn);
			return false;
		}

		if(p) {
			*p = E_DIR_SEPARATOR;
			p++;

			while(*p && *p == E_DIR_SEPARATOR)
				p++;
		}
	} while(p);

	free(fn);
	return true;
}

bool dir_remove(const char* name) {
	E_ASSERT(name != NULL);
	return (rmdir(name) == 0);
}

bool dir_rename(const char* from, const char* to) {
	E_ASSERT(from != NULL);
	E_ASSERT(to != NULL);

	return (rename(from, to) == 0);
}

/* stolen from coreutils (cp,mv) package */
static struct dirent *readdir_ignoring_dots(DIR *dirp) {
	struct dirent *dp = NULL;

	while(1) {
		dp = readdir(dirp);
		if(dp == NULL || !DOT_OR_DOTDOT(dp->d_name))
			break;
	}

	return dp;
}

bool dir_empty(const char* name) {
	E_ASSERT(name != NULL);

	DIR *dirp = opendir(name);
	E_RETURN_VAL_IF_FAIL(dirp != NULL, false);

	errno = 0;
	struct dirent *dp = readdir_ignoring_dots(dirp);
	int saved_errno = errno;

	closedir(dirp);
	if(dp != NULL)
		return false;

	return (saved_errno == 0 ? true : false);
}

String dir_home(void) {
	char* p = getenv("HOME");
	if(E_LIKELY(p)) return p;

	/*
	 * Fallback, read passwd structure, firstly acquiring it with _SC_GETPW_R_SIZE_MAX, then with our own size.
	 * 
	 * Note that valgrind will report leak here, with this message:
	 *   __nss_database_lookup (in /lib/tls/libc-2.3.5.so)
	 *   getpwuid_r@@GLIBC_2.1.2 (in /lib/tls/libc-2.3.5.so)
	 *
	 * This looks like a leak in some glibc versions.
	 */
	long bufsz = 1024;

#ifdef _SC_GETPW_R_SIZE_MAX
	long b = sysconf(_SC_GETPW_R_SIZE_MAX);
	if(b > 0) bufsz = b;
#endif

	struct passwd pwd_str, *pw = NULL;
	char *buf = new char[bufsz];
	int err = getpwuid_r(getuid(), &pwd_str, buf, bufsz, &pw);

	if(err) {
		delete [] buf;
		return "";
	}

	E_ASSERT(pw != NULL);

	String ret = pw->pw_dir;
	delete [] buf;

	return ret;
}

String dir_current(void) {
	char buf[PATH_MAX];
	return getcwd(buf, PATH_MAX) ? buf : "";
}

bool dir_list(const char *dir, list<String> &lst, bool full_path, bool show_hidden, bool show_dots) {
	E_ASSERT(dir != NULL);

	DIR *dirp = opendir(dir);
	E_RETURN_VAL_IF_FAIL(dirp != NULL, false);

	/* make sure the list is empty */
	lst.clear();

	String dirstr, tmp;
	if(full_path) {
		/* resolve full name if given folder in form: './file' */
		dirstr = (dir[0] == '.' && dir[1] == '\0') ? dir_current() : dir;

		if(!str_ends(dirstr.c_str(), E_DIR_SEPARATOR_STR))
			dirstr += E_DIR_SEPARATOR_STR;
	}

	for(dirent *dp = readdir(dirp); dp != NULL; dp = readdir(dirp)) {
		if(!show_hidden && dp->d_name[0] == '.' && !DOT_OR_DOTDOT(dp->d_name))
			continue;

		if(!show_dots && DOT_OR_DOTDOT(dp->d_name))
			continue;

		if(full_path) {
			tmp = dirstr;
			tmp += dp->d_name;
			lst.push_back(tmp);
		} else {
			lst.push_back(dp->d_name);
		}
	}

	lst.sort();
	closedir(dirp);
	return true;
}

EDELIB_NS_END
