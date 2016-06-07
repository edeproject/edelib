/*
 * $Id$
 *
 * Temporary file handler
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

#ifndef __EDELIB_TEMPFILE_H__
#define __EDELIB_TEMPFILE_H__

#include <stdio.h>
#include "String.h"

EDELIB_NS_BEGIN

/**
 * \class TempFile
 * \brief Temporary file class
 *
 * TempFile is responsible for creating unique temporary file.
 *
 * TempFile behaves much as mktemp() or mkstemp() functions: you provide location with <i>templated</i>
 * name, and it will try to create the one, by opening it (like mkstemp()). Templated name ends with XXXXXX.
 * On other hand, if you do not provide XXXXXX-es at the end, TempFile will append them.
 *
 * This class was greatly inspired by KTempFile from kdelibs because I had no clue how to model it ;)
 */
class EDELIB_API TempFile {
private:
	int     fd;
	int     errno_err;
	FILE   *stream;
	bool    auto_del;
	bool    no_close;
	String  filename;
	
	E_DISABLE_CLASS_COPY(TempFile)
public:
	/**
	 * Constructor. Does nothing, except setting internal values to invalid state
	 */
	TempFile();

	/**
	 * Destructor. Closes file descriptor too
	 */
	~TempFile();

	/**
	 * Create file and return creation status. <i>prefix</i> is location where file will be 
	 * created, with given mode. If file could not be created at all, status() will return <i>errno</i>
	 * value for further inspection
	 */
	bool create(const char* prefix, int mode = 0600);

	/**
	 * Check if create() was successfull
	 */
	operator bool(void) const { return (errno_err == 0); }

	/**
	 * Removes the file. If file wasn't created successfully before, it will do nothing
	 */
	void unlink(void);

	/**
	 * Closes descriptor
	 */
	bool close(void);

	/**
	 * If set to true, the file will be deleted when TempFile destructor was called. Default is false
	 */
	void set_auto_delete(bool v) { auto_del = v; }

	/**
	 * If set to true, the file descriptors will not be closed when destructor was called. Default is false
	 */
	void set_no_close(bool n) { no_close = n; }

	/**
	 * Return FILE object or NULL if create() failed
	 */
	FILE* fstream(void);

	/**
	 * Return temporary file name. If create() failed, NULL will be returned
	 */
	const char* name(void) const { return (filename.empty() ? NULL : filename.c_str()); }

	/**
	 * Return file descriptor
	 */
	int handle(void) const { return fd; }

	/**
	 * Return errno status
	 */
	int status(void) const { return errno_err; }
};

EDELIB_NS_END
#endif
