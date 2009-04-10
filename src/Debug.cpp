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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <edelib/Debug.h>

#define ERROR_BUFLEN 256

/* in case namespace was changed or disabled */
EDELIB_NS_USE

typedef void (*MessageHandlerType)(ErrorMessageType t, const char* msg);
static MessageHandlerType default_msg_handler = 0;

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
	fprintf(err, "----- stack dump -----\n"); \
	__dump_stack(fd);                         \
	fprintf(err, "----------------------\n")
#else
#define DUMP_STACK(err, fd)
#endif

void _edelib_debug(const char* fmt, ...) {
	char buf[ERROR_BUFLEN];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buf, ERROR_BUFLEN, fmt, ap);
	va_end(ap);

	if(default_msg_handler)
		default_msg_handler(ERROR_MESSAGE_DEBUG, buf);
	else
		fputs(buf, stderr);
}

void _edelib_warning(const char* fmt, ...) {
	char buf[ERROR_BUFLEN];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buf, ERROR_BUFLEN, fmt, ap);
	va_end(ap);

	if(default_msg_handler)
		default_msg_handler(ERROR_MESSAGE_WARNING, buf);
	else
		fputs(buf, stderr);
}

void _edelib_assert(int cond, const char* cond_text, const char* file, int line, const char* func) {
	if(!cond)	
		_edelib_fatal("Assertion failed: \"%s\" in %s (%d), function: \"%s\"\n", cond_text, file, line, func);
}

void _edelib_fatal(const char* fmt, ...) {
	char buf[ERROR_BUFLEN];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buf, ERROR_BUFLEN, fmt, ap);
	va_end(ap);

	if(default_msg_handler)
		default_msg_handler(ERROR_MESSAGE_FATAL, buf);
	else {
		fputs(buf, stderr);
		/*
		 * glibc redefined stderr pointing it to internal IO struct, so here is used 
		 * good-old-defalut-normal-every_one_use_it alias for it. Yuck!
		 */
		DUMP_STACK(stderr, 2);
		abort();
	}
}

void error_mesage_handler_install(MessageHandlerType m) {
	default_msg_handler = m;
}
