/*
 * $Id$
 *
 * File IO stream
 * Part of edelib.
 * Copyright (c) 2005-2006 EDE Authors.
 *
 * This program is licenced under terms of the 
 * GNU General Public Licence version 2 or newer.
 * See COPYING for details.
 */

#ifndef __FILE_H__
#define __FILE_H__

#include "econfig.h"
#include <stdio.h> // FILE

EDELIB_NAMESPACE {

enum FileErrors
{
	FILE_SUCCESS = 0,  ///< successful operation
	FILE_EACCESS,      ///< permission denied
	FILE_ENOENT,       ///< no such file
	FILE_EMFILE,       ///< too many opened files
	FILE_ENSPC         ///< no space left on device
};

enum FileOpenMode
{
	FIO_R = 0,        ///< open file in read-only mode
	FIO_W,            ///< open file in write mode, and truncate it to zero length
	FIO_A,            ///< open file in append mode
	FIO_WC            ///< open file in write mode, and create it, if does not exists
};

/*! \class File
 * A system file io stream.
 *
 * File is portable wrapper for various
 * functions for reading and writing files.
 * Also brings automatic descriptor closing
 * or possible another file openning during
 * object lifetime.
 *
 * Here is a sample of common usage:
 * \code
 *  File f("foo.txt", FIO_R);
 *  while(!f.eof())
 *  	printf("%s", f.readline());
 *  f.close();
 * \endcode
 */

class File
{
	private:
		FILE* fobj;

		File(const File&);
		File& operator=(File&);

	public:
		/**
		 * Creates empty file object.
		 */
		File();

		/**
		 * Cleans reserved data, and closes
		 * all possible opened descriptors.
		 */
		~File();

		/**
		 * Open file in given mode
		 *
		 * \param fname file name to open
		 * \param mode how to open file; default is read-only
		 */
		bool open(const char* fname, FileOpenMode mode);

		/**
		 * Close current open descriptor.
		 * If is not opened, function will do nothing.
		 */
		void close();

		/**
		 * Checks if end of file reached.
		 *
		 * \return true if end of file is reached.
		 */
		bool eof(void);

		/**
		 * Read line from stream in specified buffer, with given
		 * size. If buffer size is less than read line, only
		 * given size will be filled. A '\\0' is stored as last
		 * character in buffer.
		 *
		 * \return size of readed data
		 * \param buff where to place content
		 * \param buffsz size of buffer
		 */
		int readline(char* buff, int buffsz);

		/**
		 * Write data to the stream, with specified size.
		 *
		 * \return number of written data
		 * \param buff data to write
		 * \param buffsz size of buffer
		 */
		int write(const char* buff, int buffsz);
};

}

#endif
