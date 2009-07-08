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

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <edelib/TempFile.h>
#include <edelib/File.h>
#include <edelib/StrUtil.h>
#include <edelib/Debug.h>

EDELIB_NS_BEGIN

TempFile::TempFile() : fd(-1), errno_err(0), stream(0), auto_del(false)
{ }

TempFile::~TempFile() {
	TempFile::close();

	if(auto_del)
		unlink();
}

bool TempFile::create(const char* prefix, int mode) {
	int len = strlen(prefix);
	bool has_xxxxxx = false;

	if(str_ends(prefix, "XXXXXX"))
		has_xxxxxx = true;
	else
		len += 6;

	char* tmp = new char[len + 1];
	strcpy(tmp, prefix);

	/* append it if is missing */
	if(!has_xxxxxx)
		strcat(tmp, "XXXXXX");

	errno = 0;

	fd = mkstemp(tmp);
	if(fd == -1) {
		errno_err = errno;
		E_WARNING(E_STRLOC ": mkstemp() failed on '%s': (%i) %s\n", tmp, errno_err, strerror(errno_err));

		delete [] tmp;
		return false;
	}

	/* stolen from KTempFile.cpp */
	mode_t t = 0, u = 0;
	u = umask(t);
	umask(u);
	fchmod(fd, mode & (~u));

	filename = tmp;
	delete [] tmp;

	return true;
}

void TempFile::unlink(void) {
	E_RETURN_IF_FAIL(!filename.empty());

	TempFile::close();
	file_remove(filename.c_str());
}

bool TempFile::close(void) {
	if(fd == -1)
		return false;

	errno = 0;
	int ret = 0;

	if(stream)
		ret = fclose(stream);
	else
		ret = ::close(fd);

	fd = -1;
	stream = 0;

	if(ret != 0) {
		errno_err = errno;
		E_WARNING(E_STRLOC ": Unable to close() '%s' stream correctly: (%i) %s\n", errno_err, strerror(errno_err));
	}

	return (errno_err != 0);
}

FILE* TempFile::fstream(void) {
	E_RETURN_VAL_IF_FAIL(fd != -1, NULL);

	errno = 0;

	stream = fdopen(fd, "r+");
	if(!stream) {
		errno_err = errno;
		E_WARNING(E_STRLOC ": Unable to fdopen() '%s' stream correctly: (%i) %s\n", errno_err, strerror(errno_err));
	}

	return stream;
}

EDELIB_NS_END
