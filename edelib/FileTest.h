/*
 * $Id$
 *
 * File info facility
 * Copyright (c) 2009 edelib authors
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

#ifndef __FILETEST_H__
#define __FILETEST_H__

#include "edelib-global.h"

EDELIB_NS_BEGIN

/**
 * \enum FileTestFlags
 * \brief Available flags for file_test()
 */
typedef enum {
	FILE_TEST_IS_REGULAR    = 1 << 0,   ///< file
	FILE_TEST_IS_DIR        = 1 << 1,   ///< directory
	FILE_TEST_IS_SYMLINK    = 1 << 2,   ///< symbolic link
	FILE_TEST_IS_CHAR       = 1 << 3,   ///< character device
	FILE_TEST_IS_BLOCK      = 1 << 4,   ///< block device
	FILE_TEST_IS_FIFO       = 1 << 5,   ///< FIFO (pipes)
	FILE_TEST_IS_SOCKET     = 1 << 6,   ///< socket
	FILE_TEST_IS_READABLE   = 1 << 7,   ///< readable
	FILE_TEST_IS_WRITEABLE  = 1 << 8,   ///< writeable
	FILE_TEST_IS_EXECUTABLE = 1 << 9,   ///< executable
	FILE_TEST_EXISTS        = 1 << 10   ///< exists, no matter what type
} FileTestFlags;

/**
 * file_test() is generic checker for target file type on the system, where file type could be directory, regular file
 * socket and etc. This function, besides deducing type, can also check for file's existance and it's access flags.
 *
 * <em>flags</em> (\see FileTestFlags) should be set to specify what to test. These values can be OR-ed, so if you
 * wan't to see if target object is file and is executable, you will use FILE_TEST_IS_REGULAR | FILE_TEST_IS_EXECUTABLE.
 *
 * Care must be taken when combine certain flags. For example, since file can be readable, writeable and executable,
 * combination like FILE_TEST_IS_READABLE | FILE_TEST_IS_WRITEABLE | FILE_TEST_IS_EXECUTABLE | FILE_TEST_IS_REGULAR is
 * perfectly valid. On other hand, FILE_TEST_IS_DIR | FILE_TEST_IS_REGULAR makes no sense, so only first bits
 * will be considered (FILE_TEST_IS_DIR only), and these flags will yield false from file_test().
 */
EDELIB_API bool file_test(const char* path, unsigned int flags);

EDELIB_NS_END

#endif
