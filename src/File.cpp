/*
 * $Id$
 *
 * File IO stream
 * Copyright (c) 2005-2007 edelib authors
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <utime.h>
#include <unistd.h>
#include <stdio.h>

#include <edelib/File.h>
#include <edelib/FileTest.h>
#include <edelib/Debug.h>
#include <edelib/StrUtil.h>
#include <edelib/List.h>

EDELIB_NS_BEGIN

bool file_exists(const char* name) {
	E_ASSERT(name != NULL);
	struct stat s;
	// FIXME: stat or lstat
	if(stat(name, &s) != 0)
		return false;
	return (access(name, F_OK) == 0) && S_ISREG(s.st_mode);
}

bool file_readable(const char* name) {
	E_ASSERT(name != NULL);
	struct stat s;
	// FIXME: stat or lstat
	if(stat(name, &s) != 0)
		return false;
	return (access(name, R_OK) == 0) && S_ISREG(s.st_mode);
}

bool file_writeable(const char* name) {
	E_ASSERT(name != NULL);
	struct stat s;
	// FIXME: stat or lstat
	if(stat(name, &s) != 0)
		return false;
	return (access(name, W_OK) == 0) && S_ISREG(s.st_mode);
}

bool file_executable(const char* name) {
	E_ASSERT(name != NULL);
	struct stat s;
	// FIXME: stat or lstat
	if(stat(name, &s) != 0)
		return false;
	return (access(name, R_OK) == 0) && S_ISREG(s.st_mode) && (s.st_mode & S_IXUSR);
}

bool file_remove(const char* name) {
	E_ASSERT(name != NULL);

	return (unlink(name) == 0);
}

bool file_copy(const char* src, const char* dest, bool exact) {
	E_ASSERT(src != NULL);
	E_ASSERT(dest != NULL);

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
	E_ASSERT(from != NULL);
	E_ASSERT(to != NULL);

	return (rename(from, to) == 0);
}

String file_path(const char* fname, bool skip_link) {
	E_ASSERT(fname != NULL);

	if(file_test(fname, FILE_TEST_IS_REGULAR | FILE_TEST_IS_EXECUTABLE))
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

	const char* sptr;
	for(; it != it_end; ++it) {
		/* assume PATH does not contains entries ending with '/' */
		(*it) += '/';
		(*it) += fname;
		sptr = (*it).c_str();

		if(skip_link == false && file_test(sptr, FILE_TEST_IS_REGULAR | FILE_TEST_IS_EXECUTABLE))
			return *it;

		if(skip_link == true && 
				file_test(sptr, FILE_TEST_IS_REGULAR | FILE_TEST_IS_EXECUTABLE) && 
				file_test(sptr, FILE_TEST_IS_SYMLINK) == false)
		{
			return *it;
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
	E_ASSERT(name != NULL && "File name is NULL");

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
	E_ASSERT(opened != false && "File stream not opened");

	return feof(fobj);
}

int File::getch(void) {
	E_ASSERT(opened != false && "File stream not opened");

	return fgetc(fobj);
}

int File::putch(int c) {
	E_ASSERT(opened != false && "File stream not opened");
	E_ASSERT((have_flag(FIO_WRITE, fmode) || have_flag(FIO_APPEND, fmode)) && "File stream not in write mode");

	return fputc(c, fobj);
}
int File::read(void* buff, int typesz, int buffsz) {
	E_ASSERT(opened != false && "File stream not opened");
	E_ASSERT(buff != NULL);

	return fread(buff, typesz, buffsz, fobj);
}

/*
 * same sa fgets, but return len of line
 */
int File::readline(char* buff, int buffsz) {
	E_ASSERT(opened != false && "File stream not opened");
	E_ASSERT(buff != NULL);

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
	E_ASSERT(opened != false && "File stream not opened or not opened in write mode");
	E_ASSERT((have_flag(FIO_WRITE, fmode) || have_flag(FIO_APPEND, fmode)) && "File stream not in write mode");

	return fwrite(buff, typesz, buffsz, fobj);
}

int File::write(const char* buff, unsigned int buffsz) {
	// assure we don't write data out of the bounds
	E_ASSERT(strlen(buff)+1 >= buffsz && "Buffer size is greater than actual size");

	return write(buff, 1, buffsz);
}

int File::write(const char* buff) {
	return write(buff, 1, strlen(buff));
}

int File::printf(const char* fmt, ...) {
	E_ASSERT(opened != false && "File stream not opened or not opened in write mode");
	E_ASSERT((have_flag(FIO_WRITE, fmode) || have_flag(FIO_APPEND, fmode)) && "File stream not in write mode");
	E_ASSERT(fmt != NULL);

	va_list ap;
	va_start(ap, fmt);
	int ret = vfprintf(fobj, fmt, ap);
	va_end(ap);
	return ret;
}

EDELIB_NS_END
