/*
 * $Id: String.h 2594 2009-03-25 14:54:54Z karijes $
 *
 * EDE specific code
 * Copyright (c) 2010-2012 edelib authors
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

#include <edelib/Ede.h>
#include <edelib/Debug.h>
#include <stdlib.h>
#include <syslog.h>
#include <stdio.h>

/* various flags used internally */
#define EDE_BOOTSTRAP_SYSLOG (1 << 1)

EDELIB_NS_BEGIN

static void syslog_logger(int t, const char *domain, const char *msg) {
	int pr;
	const char *str;

	switch(t) {
		case EDELIB_ERROR_MESSAGE_WARNING:
			pr = LOG_WARNING;
			str = "warning";
			break;
		case EDELIB_ERROR_MESSAGE_FATAL:
			pr = LOG_CRIT;
			str = "fatal";
			break;
		case EDELIB_ERROR_MESSAGE_DEBUG:
		default:
			pr = LOG_INFO;
			str = "debug";
			break;
	}

	if(domain)
		syslog(pr, "[%s] (%s) %s", domain, str, msg);
	else
		syslog(pr, "(%s) %s", str, msg);
}

ApplicationBootstrap::ApplicationBootstrap(const char *name, const char *path) : flags(0) {
	nls_support_init(name, path);

	/* enable system logger if set */
	if(getenv("EDE_SYSLOG_ENABLE"))
		flags |= EDE_BOOTSTRAP_SYSLOG;

	if(flags & EDE_BOOTSTRAP_SYSLOG) {
		setlogmask(LOG_UPTO (LOG_INFO));
		openlog("ede", LOG_CONS | LOG_PID, LOG_USER);
		edelib_error_message_handler_install(syslog_logger);
	}
}

ApplicationBootstrap::~ApplicationBootstrap() {
	if(flags & EDE_BOOTSTRAP_SYSLOG)
		closelog();
}

EDELIB_NS_END
