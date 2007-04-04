/*
 * $Id$
 *
 * Part of edelib.
 * Copyright (c) 2005-2007 EDE Authors.
 *
 * This program is licenced under terms of the 
 * GNU General Public Licence version 2 or newer.
 * See COPYING for details.
 */

#include <edelib/Debug.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define ERROR_BUFFLEN 1024

// in case we wan't to disable namespaces
#define EMsgHandlerType   EDELIB_NS::MsgHandlerType
#define EMsgDebug         EDELIB_NS::MsgDebug
#define EMsgWarn          EDELIB_NS::MsgWarn
#define EMsgFatal         EDELIB_NS::MsgFatal

EMsgHandlerType default_msg_handler = 0;

void InstallMsgHandler(EMsgHandlerType m)
{
	default_msg_handler = m;
}

void EDebug(const char* fmt, ...)
{
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

void EWarning(const char* fmt, ...)
{
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

void EFatal(const char* fmt, ...)
{
	char buff[ERROR_BUFFLEN];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buff, ERROR_BUFFLEN, fmt, ap);
	va_end(ap);

	if(default_msg_handler)
		default_msg_handler(EMsgFatal, buff);
	else
	{
		fprintf(stderr, "%s", buff);
		abort();
	}
}
