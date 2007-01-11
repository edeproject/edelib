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

#include <string.h> // strlen, strncpy
#include <stdarg.h> // va_xxx stuff, vfprintf

#include <sys/types.h> // stat
#include <sys/stat.h>  // stat
#include <unistd.h>    // access, stat

EDELIB_NAMESPACE {

bool file_exists(const char* name)
{
	EASSERT(name != NULL);
	struct stat s;
	if(stat(name, &s) != 0)
		return false;
	return (access(name, F_OK) == 0) && S_ISREG(s.st_mode);
}

bool file_readable(const char* name)
{
	EASSERT(name != NULL);
	struct stat s;
	if(stat(name, &s) != 0)
		return false;
	return (access(name, R_OK) == 0) && S_ISREG(s.st_mode);
}

bool file_writeable(const char* name)
{
	EASSERT(name != NULL);
	struct stat s;
	if(stat(name, &s) != 0)
		return false;
	return (access(name, W_OK) == 0) && S_ISREG(s.st_mode);
}

inline bool have_flag(FileIOMode m, int flags)
{
	return ((flags & m) == m) ? true : false;
}

File::File() : fobj(NULL), fname(NULL), fmode(0), errcode(FILE_ENOENT), opened(false), alloc(false)
{
}

File::~File()
{
	close();
}

File::File(const char* name, int mode)
{
	// state flags are already set
	open(name, mode);
}

/*
 * it will open file via C stdio facility
 */
bool File::open(const char* name, int mode)
{
	EASSERT(name != NULL && "File name is NULL");
	const char* flags;
	switch(mode)
	{
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
	if(!fobj)
	{
		// TODO: add error codes
		return false;
	}

	opened = true;
	return true;
}

void File::close(void)
{
	if(alloc)
	{
		delete [] fname;
		fname = NULL;
		alloc = false;
	}

	if(!opened)
		return;

	fclose(fobj);
	opened = false;
}

const char* File::name(void) const
{
	return fname;
}

bool File::eof(void)
{
	EASSERT(opened != false && "File stream not opened");
	return feof(fobj);
}

int File::getch(void)
{
	EASSERT(opened != false && "File stream not opened");
	return fgetc(fobj);
}

int File::putch(int c)
{
	EASSERT(opened != false && "File stream not opened");
	EASSERT(have_flag(FIO_WRITE, fmode)||have_flag(FIO_APPEND, fmode) && "File stream not in write mode");
	return fputc(c, fobj);
}
int File::read(void* buff, int typesz, int buffsz)
{
	EASSERT(opened != false && "File stream not opened");
	EASSERT(buff != NULL);
	return fread(buff, typesz, buffsz, fobj);
}

/*
 * same sa fgets, but return len of line
 */
int File::readline(char* buff, int buffsz)
{
	EASSERT(opened != false && "File stream not opened");
	EASSERT(buff != NULL);

	int i = 0;
	int c;
	char* buffp = buff;
	for(; i < buffsz; i++)
	{
		c = fgetc(fobj);
		if(c == EOF)
		{
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

int File::write(const void* buff, int typesz, int buffsz)
{
	EASSERT(opened != false && "File stream not opened or not opened in write mode");
	EASSERT(have_flag(FIO_WRITE, fmode)||have_flag(FIO_APPEND, fmode) && "File stream not in write mode");

	return fwrite(buff, typesz, buffsz, fobj);
}

int File::write(const char* buff, unsigned int buffsz)
{
	// assure we don't write data out of the bounds
	EASSERT(strlen(buff)+1 >= buffsz && "Buffer size is greater than actual size");

	return write(buff, 1, buffsz);
}

int File::printf(const char* fmt, ...)
{
	EASSERT(opened != false && "File stream not opened or not opened in write mode");
	EASSERT(have_flag(FIO_WRITE, fmode)||have_flag(FIO_APPEND, fmode) && "File stream not in write mode");
	EASSERT(fmt != NULL);

	va_list ap;
	va_start(ap, fmt);
	int ret = vfprintf(fobj, fmt, ap);
	va_end(ap);
	return ret;
}
}
