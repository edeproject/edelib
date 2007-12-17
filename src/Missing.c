/*
 * $Id$
 *
 * Provide missing functions
 * Part of edelib.
 * Copyright (c) 2005-2007 EDE Authors.
 *
 * This program is licensed under terms of the 
 * GNU General Public Licence version 2 or newer.
 * See COPYING for details.
 */

#include <edelib/Missing.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#if 0
/* left for the future */
int edelib_putenv(char* name) {
	extern char** environ;

	unsigned int len;
	const char** eptr;
	char** new_environ;
	int ecount;
	const char* eq = (const char*)strchr(name, '=');

	if(!eq)
		len = strlen(name);
	else
		len = eq - name + 1;

	eptr = (const char**)environ;
	for(ecount = 0; *eptr; eptr++, ecount++) {
		if(*name == **eptr && !memcmp(name, *eptr, len)) {
			/* if '=' not found, then remove variable */
			if(!eq) {
				for(; eptr[1]; eptr++)
					eptr[0] = eptr[1];
				eptr[0] = 0;
				return 0;
			}

			/* variable found in array, just update it and quit */
			*eptr = name;
			return 0;
		}
	}

	/* not found, allocate space for another pointer */
	if(eq) {
		new_environ = (char**)malloc((ecount + 2) * sizeof(char*));
		if(!new_environ)
			return -1;
		new_environ[0] = (char*)name;
		memcpy(new_environ + 1, environ, (ecount + 1) * sizeof(char*));
		environ = new_environ;
	}

	return 0;
}
#endif

int edelib_setenv(const char* name, const char* value, int overwrite) {
#ifdef HAVE_SETENV
	return setenv(name, value, overwrite);
#else
	char* nval;

	if(!name || name == '\0' || strchr(name, '=') != NULL) {
		errno = EINVAL;
		return -1;
	}

	if(overwrite == 0 && getenv(name))
		return 0;

	if(!value)
		value = "";

	nval = (char*)malloc(strlen(name) + strlen(value) + 2);
	if(!nval)
		return -1;

	sprintf(nval, "%s=%s", name, value);
	return putenv(nval);
#endif
}

int edelib_unsetenv(const char* name) {
#ifdef HAVE_UNSETENV
	return unsetenv(name);
#else
	extern char** environ;
	char** eptr;
	unsigned int len;

	if(!name || name == '\0' || strchr(name, '=') != NULL) {
		errno = EINVAL;
		return -1;
	}

	len = strlen(name);

	for(eptr = environ; *eptr; eptr++) {
		if(strncmp(*eptr, name, len) == 0 && (*eptr)[len] == '=') {
			/* remove variable and continue in case it appears again */
			for(; eptr[1]; eptr++)
				eptr[0] = eptr[1];
			eptr[0] = 0;
		}
	}

	return 0;
#endif
}

unsigned int edelib_strnlen(const char* str, unsigned int maxlen) {
#ifdef HAVE_STRNLEN
	return strnlen(str, maxlen);
#else
	const char* p;
	unsigned int s;

	if(str == NULL)
		return 0;

	p = str;
	for(s = 0; *p != '\0' && s < maxlen; p++, s++)
		;
	return s;
#endif
}

char* edelib_strndup(const char* str, unsigned int maxlen) {
#ifdef HAVE_STRNDUP
	return strndup(str, maxlen);
#else
	unsigned int len = edelib_strnlen(str, maxlen);
	char* nstr = (char*)malloc(len + 1);

	if(nstr == NULL)
		return NULL;

	nstr[len] = '\0';
	return (char*)memcpy(nstr, str, len);
#endif
}
