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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <edelib/FileTest.h>
#include <edelib/Debug.h>

EDELIB_NS_BEGIN

bool file_test(const char* path, unsigned int flags) {
	E_ASSERT(path != NULL);

	bool ret = false;

	if(flags & (FILE_TEST_EXISTS |
				FILE_TEST_IS_READABLE |
				FILE_TEST_IS_EXECUTABLE |
				FILE_TEST_IS_WRITEABLE))
	{
		int mask = 0;

		if(flags & FILE_TEST_EXISTS)
			mask |= F_OK;

		if(flags & FILE_TEST_IS_READABLE)
			mask |= R_OK;

		if(flags & FILE_TEST_IS_EXECUTABLE)
			mask |= X_OK;

		if(flags & FILE_TEST_IS_WRITEABLE)
			mask |= W_OK;

		ret = (access(path, mask) == 0);
	}

	if(flags & (FILE_TEST_IS_REGULAR |
				FILE_TEST_IS_DIR |
				FILE_TEST_IS_SYMLINK |
				FILE_TEST_IS_CHAR |
				FILE_TEST_IS_BLOCK |
				FILE_TEST_IS_FIFO |
				FILE_TEST_IS_SOCKET))
	{
		struct stat st;

		if(flags & FILE_TEST_IS_SYMLINK && lstat(path, &st) == 0) {
			ret = (S_ISLNK(st.st_mode) == 1);
		} else if(stat(path, &st) == 0) {
			if(flags & FILE_TEST_IS_REGULAR)
				ret = (S_ISREG(st.st_mode) == 1);
			else if(flags & FILE_TEST_IS_DIR)
				ret = (S_ISDIR(st.st_mode) == 1);
			else if(flags & FILE_TEST_IS_CHAR)
				ret = (S_ISCHR(st.st_mode) == 1);
			else if(flags & FILE_TEST_IS_BLOCK)
				ret = (S_ISBLK(st.st_mode) == 1);
			else if(flags & FILE_TEST_IS_FIFO)
				ret = (S_ISFIFO(st.st_mode) == 1);
			else if(flags & FILE_TEST_IS_SOCKET)
				ret = (S_ISSOCK(st.st_mode) == 1);
		} else {
			ret = false;
		}
	}

	return ret;
}

EDELIB_NS_END
