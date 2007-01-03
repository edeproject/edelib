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
	FILE_ENSPC,        ///< no space left on device
	FILE_FLAG          ///< bad flag
};

enum FileIOMode
{
	FIO_READ    = (1<<1),          ///< open file in read-only mode
	FIO_WRITE   = (1<<2),          ///< open file in write mode, and truncate it to zero length
	FIO_APPEND  = (1<<3),          ///< open file in append mode
	FIO_BINARY  = (1<<4),          ///< open file in binary mode
	FIO_TRUNC   = (1<<5)           ///< truncate currently opened file
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
 *  File f("foo.txt");
 *  if(!f)
 *  	printf("can't open %s\n", f.name());
 *  while(!f.eof())
 *  	printf("%s", f.readline());
 *  f.close();
 * \endcode
 */

class File
{
	private:
		FILE* fobj;
		char* fname;
		int fmode;
		int errcode;
		bool opened;
		bool alloc;

		File(const File&);
		File& operator=(File&);

	public:
		/**
		 * Creates empty file object.
		 */
		File();

		/**
		 * Open stream for reading or writing
		 *
		 * \param fname file name to open
		 * \param mode how to open file; default is read-only
		 */
		File(const char* fname, int mode);

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
		bool open(const char* fname, int mode = FIO_READ);

		/**
		 * Close current open descriptor.
		 * If is not opened, function will do nothing.
		 */
		void close(void);

		/**
		 * Returns name of curent opened stream
		 *
		 * \return name of curent opened stream
		 */
		const char* name(void) const;

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
		 * \param typesz size of each item
		 * \param buffsz size of buffer
		 */
		int write(const void* buff, int typesz, int buffsz);

		/**
		 * Write char data to the stream.
		 *
		 * \return number of written data
		 * \param buff data to write
		 * \param buffsz size of buffer
		 */
		int write(const char* buff, unsigned int buffsz);
};

/*! \addtogroup functions
 * @{
 */
/** Check if file exists */
bool file_exists(const char* name);

/** Check if file is readable */
bool file_readable(const char* name);

/** Check if file is writeable */
bool file_writeable(const char* name);
/*! @} */
}

#endif
