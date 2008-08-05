/*
 * $Id$
 *
 * File IO stream
 * Part of edelib.
 * Copyright (c) 2005-2007 EDE Authors.
 *
 * This program is licenced under terms of the 
 * GNU General Public Licence version 2 or newer.
 * See COPYING for details.
 */

#include <edelib/econfig.h>
#include <edelib/File.h>
#include <edelib/Debug.h>
#include <edelib/StrUtil.h>
#include <edelib/List.h>

#include <stdlib.h> // getenv
#include <string.h> // strlen, strncpy
#include <stdarg.h> // va_xxx stuff, vfprintf

#include <sys/types.h> // stat, chmod, utime
#include <sys/stat.h>  // stat, chmod
#include <utime.h>     // utime
#include <unistd.h>    // access, stat, unlink

#include <stdio.h>     // rename

EDELIB_NS_BEGIN

bool file_exists(const char* name) {
	EASSERT(name != NULL);
	struct stat s;
	// FIXME: stat or lstat
	if(stat(name, &s) != 0)
		return false;
	return (access(name, F_OK) == 0) && S_ISREG(s.st_mode);
}

bool file_readable(const char* name) {
	EASSERT(name != NULL);
	struct stat s;
	// FIXME: stat or lstat
	if(stat(name, &s) != 0)
		return false;
	return (access(name, R_OK) == 0) && S_ISREG(s.st_mode);
}

bool file_writeable(const char* name) {
	EASSERT(name != NULL);
	struct stat s;
	// FIXME: stat or lstat
	if(stat(name, &s) != 0)
		return false;
	return (access(name, W_OK) == 0) && S_ISREG(s.st_mode);
}

bool file_executable(const char* name) {
	EASSERT(name != NULL);
	struct stat s;
	// FIXME: stat or lstat
	if(stat(name, &s) != 0)
		return false;
	return (access(name, R_OK) == 0) && S_ISREG(s.st_mode) && (s.st_mode & S_IXUSR);
}

bool file_remove(const char* name) {
	EASSERT(name != NULL);

	if(file_exists(name)) {
		int ret = unlink(name);

		if(ret == 0)
			return true;
		else
			return false;
	}

	return false;
}

bool file_copy(const char* src, const char* dest, bool exact) {
	EASSERT(src != NULL);
	EASSERT(dest != NULL);

	FILE* f1 = NULL;
	if((f1 = fopen(src, "rb")) == NULL)
		return false;

	FILE* f2 = NULL;
	if((f2 = fopen(dest, "wb")) == NULL) {
		fclose(f1);
		return false;
	}

	int ch;

	// FIXME: should we fail if putc returns -1 ???
	while((ch = getc(f1)) != EOF)
		putc(ch, f2);

	fclose(f1);
	fclose(f2);

	/*
	 * Now try to copy permissions via stat and set it to dest.
	 * Do the same for date/time. If fails, we see is as whole failed
	 * operation and will return false.
	 */
	if(exact) {
		struct stat s;
		// pickup source (only interested in mode_t for chmod() and st_atime/st_mtime for utime)
		if(stat(src, &s) != 0)
			return false;

		// set permissions
		if(chmod(dest, s.st_mode) != 0)
			return false;

		struct utimbuf tbuff;
		tbuff.actime = s.st_atime;
		tbuff.modtime = s.st_mtime;

		// set time
		if(utime(dest, &tbuff) != 0)
			return false;
	}

	return true;
}

bool file_rename(const char* from, const char* to) {
	EASSERT(from != NULL);
	EASSERT(to != NULL);

	if(!file_exists(from))
		return false;

	int ret = rename(from, to);
	if(ret == 0)
		return true;

	return false;
}

String file_path(const char* fname, bool check_link) {
	EASSERT(fname != NULL);

	if(file_exists(fname))
		return fname;

	char* val = getenv("PATH");
	if(!val)
		return "";

	list<String> vs;
	stringtok(vs, val, ":");

	if(!vs.size())
		return "";

	list<String>::iterator it, it_end;
	it = vs.begin();
	it_end = vs.end();

	struct stat s;
	const char* sptr;
	for(; it != it_end; ++it) {
		// assume PATH does not contains entries ending with '/'
		(*it) += '/';
		(*it) += fname;
		sptr = (*it).c_str();

		if(file_exists(sptr)) {
			if(!check_link) 
				return (*it);
			else if((lstat(sptr, &s) == 0) && !S_ISLNK(s.st_mode)) {
				return (*it);
			}
		}
	}

	return "";
}

inline bool have_flag(FileIOMode m, int flags) {
	return ((flags & m) == m) ? true : false;
}

File::File() : fobj(NULL), fname(NULL), fmode(0), errcode(FILE_ENOENT), opened(false), alloc(false)
{
}

File::~File() {
	close();
}

File::File(const char* name, int mode) {
	// state flags are already set
	open(name, mode);
}

/*
 * it will open file via C stdio facility
 */
bool File::open(const char* name, int mode) {
	EASSERT(name != NULL && "File name is NULL");

	const char* flags;
	switch(mode) {
		case FIO_READ:
			flags = "r";
			break;
		case FIO_WRITE:
		case FIO_WRITE | FIO_TRUNC:
			flags = "w";
			break;
		case FIO_WRITE | FIO_BINARY:
		case FIO_WRITE | FIO_BINARY | FIO_TRUNC:
			flags = "wb";
			break;
		case FIO_APPEND:
		case FIO_WRITE | FIO_APPEND:
			flags = "a";
			break;
		case FIO_APPEND | FIO_BINARY:
		case FIO_WRITE | FIO_APPEND | FIO_BINARY:
			flags = "ab";
			break;
		case FIO_READ | FIO_BINARY:
			flags = "rb";
			break;
		case FIO_READ | FIO_WRITE:
			flags = "r+";
			break;
		case FIO_READ | FIO_WRITE | FIO_BINARY:
			flags = "r+b";
			break;
		case FIO_READ | FIO_WRITE | FIO_TRUNC:
			flags = "w+";
			break;
		case FIO_READ | FIO_WRITE | FIO_TRUNC | FIO_BINARY:
			flags = "w+b";
			break;
		default:
			flags = "";		// error
			errcode = FILE_FLAG;
			return false;
	}

	int sz = strlen(name);
	fname = new char[sz+1];
	strncpy(fname, name, sz+1);

	fmode = mode;
	alloc = true;
	fobj = fopen(fname, flags);
	if(!fobj) {
		// TODO: add error codes
		return false;
	}

	opened = true;
	return true;
}

void File::close(void) {

	if(alloc) {
		delete [] fname;
		fname = NULL;
		alloc = false;
	}

	if(!opened)
		return;

	fclose(fobj);
	opened = false;
}

const char* File::name(void) const {
	return fname;
}

bool File::eof(void) {
	EASSERT(opened != false && "File stream not opened");

	return feof(fobj);
}

int File::getch(void) {
	EASSERT(opened != false && "File stream not opened");

	return fgetc(fobj);
}

int File::putch(int c) {
	EASSERT(opened != false && "File stream not opened");
	EASSERT(have_flag(FIO_WRITE, fmode)||have_flag(FIO_APPEND, fmode) && "File stream not in write mode");

	return fputc(c, fobj);
}
int File::read(void* buff, int typesz, int buffsz) {
	EASSERT(opened != false && "File stream not opened");
	EASSERT(buff != NULL);

	return fread(buff, typesz, buffsz, fobj);
}

/*
 * same sa fgets, but return len of line
 */
int File::readline(char* buff, int buffsz) {
	EASSERT(opened != false && "File stream not opened");
	EASSERT(buff != NULL);

	int i = 0;
	int c;
	char* buffp = buff;
	for(; i < buffsz; i++) {
		c = fgetc(fobj);

		if(c == EOF) {
			i = EOF;
			break;
		}

		*buffp++ = c;
		if(c == '\n')
			break;
	}
	*buffp = '\0';
	return i;
}

int File::write(const void* buff, int typesz, int buffsz) {
	EASSERT(opened != false && "File stream not opened or not opened in write mode");
	EASSERT(have_flag(FIO_WRITE, fmode)||have_flag(FIO_APPEND, fmode) && "File stream not in write mode");

	return fwrite(buff, typesz, buffsz, fobj);
}

int File::write(const char* buff, unsigned int buffsz) {
	// assure we don't write data out of the bounds
	EASSERT(strlen(buff)+1 >= buffsz && "Buffer size is greater than actual size");

	return write(buff, 1, buffsz);
}

int File::write(const char* buff) {
	return write(buff, 1, strlen(buff));
}

int File::printf(const char* fmt, ...) {
	EASSERT(opened != false && "File stream not opened or not opened in write mode");
	EASSERT(have_flag(FIO_WRITE, fmode)||have_flag(FIO_APPEND, fmode) && "File stream not in write mode");
	EASSERT(fmt != NULL);

	va_list ap;
	va_start(ap, fmt);
	int ret = vfprintf(fobj, fmt, ap);
	va_end(ap);
	return ret;
}

EDELIB_NS_END
