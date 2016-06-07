/*
 * $Id$
 *
 * Debug functions
 * Copyright (c) 2005-2011 edelib authors
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
#include <edelib/Debug.h>

#define ERROR_BUFLEN 256

typedef void (*EdelibErrorHandlerType)(int t, const char* domain, const char* msg);

static void internal_logger(int type, const char *domain, const char *msg);
static EdelibErrorHandlerType do_log = internal_logger;

#ifdef __GLIBC__
#include <execinfo.h>

static void __dump_stack(int fd) {
	/* stacktrace depth calls */
	void* strace[256];
   	const int sz = backtrace(strace, (int)(sizeof(strace) / sizeof(*strace)));
	/* fd means stdout/stderr/... */
   	backtrace_symbols_fd(strace, sz, fd);
}

#define DUMP_STACK(err, fd)                   \
 fprintf(err, "----- stack dump -----\n"); \
 __dump_stack(fd);                         \
 fprintf(err, "----------------------\n")
#else
 #define DUMP_STACK(err, fd)
#endif /* __GLIBC__ */

static void internal_logger(int type, const char *domain, const char *msg) {
	if(domain)
		fprintf(stderr, "[%s] %s", domain, msg);
	else
		fprintf(stderr, "%s", msg);

	if(type == EDELIB_ERROR_MESSAGE_FATAL) {
		/*
		 * glibc redefined stderr pointing it to internal IO struct, so here is used 
		 * good-old-defalut-normal-every_one_use_it alias for it. Yuck!
		 */
		DUMP_STACK(stderr, 2);
		abort();
	}
}

void edelib_error_message_handler_install(EdelibErrorHandlerType m) {
	do_log = m;
}

void edelib_logv(const char *domain, int type, const char *fmt, va_list args) {
	char buf[ERROR_BUFLEN];
	vsnprintf(buf, ERROR_BUFLEN, fmt, args);
	do_log(type, domain, buf);
}

void edelib_log(const char *domain, int type, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	edelib_logv(domain, type, fmt, args);
	va_end(args);
}

#if !defined(EDELIB_HAVE_GNUC_VARARGS) && !defined(EDELIB_HAVE_ISO_VARARGS)
void E_DEBUG(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	edelib_logv(E_LOG_DOMAIN, EDELIB_ERROR_MESSAGE_DEBUG, fmt, args);
	va_end(args);
 }

void E_WARNING(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	edelib_logv(E_LOG_DOMAIN, EDELIB_ERROR_MESSAGE_WARNING, fmt, args);
	va_end(args);
 }

void E_FATAL(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	edelib_logv(E_LOG_DOMAIN, EDELIB_ERROR_MESSAGE_FATAL, fmt, args);
	va_end(args);
}
#endif
