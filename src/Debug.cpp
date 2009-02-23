/*
 * $Id$
 *
 * Debug functions
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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <edelib/Debug.h>

#define ERROR_BUFFLEN 1024

// in case we wan't to disable namespaces
#define EMsgHandlerType   EDELIB_NS::MsgHandlerType
#define EMsgDebug         EDELIB_NS::MsgDebug
#define EMsgWarn          EDELIB_NS::MsgWarn
#define EMsgFatal         EDELIB_NS::MsgFatal

#ifdef __GLIBC__
#include <execinfo.h>

static void __dump_stack(int fd) {
	// stacktrace depth calls
	void* strace[256];
   	const int sz = backtrace(strace, int (sizeof(strace) / sizeof(*strace)));
	// fd means stdout/stderr/...
   	backtrace_symbols_fd(strace, sz, fd);
}

#define DUMP_STACK(err, fd)                   \
	fprintf(err, "----- Stack dump -----\n"); \
	__dump_stack(fd);                         \
	fprintf(err, "----------------------\n")
#else
#define DUMP_STACK(err, fd)
#endif

static EMsgHandlerType default_msg_handler = 0;

void InstallMsgHandler(EMsgHandlerType m) {
	default_msg_handler = m;
}

/*
 * NOTE: content buffers are intentionaly declared in
 * each function due possible threaded environment
 */
void _edelib_debug(const char* fmt, ...) {
	char buff[ERROR_BUFFLEN];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buff, ERROR_BUFFLEN, fmt, ap);
	va_end(ap);

	if(default_msg_handler)
		default_msg_handler(EMsgDebug, buff);
	else
		fprintf(stderr, "%s", buff);
}

void _edelib_warning(const char* fmt, ...) {
	char buff[ERROR_BUFFLEN];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buff, ERROR_BUFFLEN, fmt, ap);
	va_end(ap);

	if(default_msg_handler)
		default_msg_handler(EMsgWarn, buff);
	else
		fprintf(stderr, "%s", buff);
}

void _edelib_assert(int cond, const char* cond_text, const char* file, int line, const char* func) {
	if(!cond)	
		_edelib_fatal("Assertion failed: \"%s\" in %s (%d), function: \"%s\"\n", cond_text, file, line, func);
}

void _edelib_fatal(const char* fmt, ...) {
	char buff[ERROR_BUFFLEN];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buff, ERROR_BUFFLEN, fmt, ap);
	va_end(ap);

	if(default_msg_handler)
		default_msg_handler(EMsgFatal, buff);
	else {
		fprintf(stderr, "%s", buff);
		/*
		 * glibc redefined stderr pointing it to internal IO struct, so here is used 
		 * good-old-defalut-normal-every_one_use_it alias for it. Yuck!
		 */
		DUMP_STACK(stderr, 2);
		abort();
	}
}
