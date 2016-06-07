/*
 * $Id$
 *
 * Regex class
 * Copyright (c) 2007-2009 edelib authors
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
#include <stdio.h>

#include <edelib/Regex.h>
#include <edelib/Debug.h>

#include "pcre/pcre.h"

#define VECTOR_COUNT 48 /* max sub expressions in PCRE, 16 * 3 */

EDELIB_NS_BEGIN

struct RegexData {
	pcre*  ppattern;
	String error;
	int    ovector[VECTOR_COUNT];
};

static int convert_match_mode(int match_mode) {
	if(match_mode == 0)
		return match_mode;

	int mode = 0;

	if(match_mode & RX_MATCH_ANCHORED)
		mode |= PCRE_ANCHORED;
	if(match_mode & RX_MATCH_NOTBOL)
		mode |= PCRE_NOTBOL;
	if(match_mode & RX_MATCH_NOTEOL)
		mode |= PCRE_NOTEOL;
	if(match_mode & RX_MATCH_NOTEMPTY)
		mode |= PCRE_NOTEMPTY;

	return mode;
}

Regex::Regex() : data(0) { }

Regex::~Regex() {
	if(data) {
		if(data->ppattern)
			pcre_free(data->ppattern);
		delete data;
	}
}

bool Regex::compile(const char* pattern, int m) {
	E_ASSERT(pattern != NULL);

	if(!data) {
		data = new RegexData;
		data->ppattern = 0;
	} else {
		if(data->ppattern)
			pcre_free(data->ppattern);
	}

	int mode = 0;

	if(m & RX_EXTENDED)
		mode |= PCRE_EXTENDED;
	if(m & RX_CASELESS)
		mode |= PCRE_CASELESS;
	if(m & RX_DOLLAR_ENDONLY)
		mode |= PCRE_DOLLAR_ENDONLY;
	if(m & RX_DOTALL)
		mode |= PCRE_DOTALL;
	if(m & RX_MULTILINE)
		mode |= PCRE_MULTILINE;
	if(m & RX_UNGREEDY)
		mode |= PCRE_UNGREEDY;

	const char* errstr;
	int			erroffset;

	data->ppattern = pcre_compile(pattern, mode, &errstr, &erroffset, NULL);

	if(data->ppattern == NULL) {
		data->error = errstr;
		return false;
	}

	return true;
}

Regex::operator bool(void) const {
	return (data != NULL && data->ppattern != NULL);
}

int Regex::match(const char* str, int match_mode, int start, int len, MatchVec* matches) {
	E_ASSERT(data != NULL && "Did you run compile() first?");
	E_ASSERT(str  != NULL);

	E_RETURN_VAL_IF_FAIL(data->ppattern != NULL, -1);

	if(len < 0)
		len = strlen(str);

	int mode = convert_match_mode(match_mode);
	int ret = pcre_exec(data->ppattern, NULL, str, len, start, mode, data->ovector, VECTOR_COUNT);

	if(ret < 1)
		return ret;

	if(matches) {
		RegexMatch m;

		for(int i = 0; i < ret; i++) {
			m.offset = data->ovector[i * 2];
			m.length = data->ovector[i * 2 + 1] - m.offset;
			matches->push_back(m);
		}
	}

	return ret;
}

int Regex::split(const char* str, list<String>& ls, int match_mode) {
	MatchVec matches;
	MatchVec::iterator it;
	String   s;
	int      ret;
	int      len = strlen(str);
	/*
	 * ppos is used to protect loop against nasty expressions which could put it to infinity 
	 * (eg. '[a-zA-Z]*' on 'abc 234 abc') -10 is random value since search() return -2 >=
	 */
	int      pos = 0, ppos = -10;

	while(1) {
		matches.clear();

		ret = match(str, match_mode, pos, len, &matches);
		if(ret < 1) {
			/* pick up the last match */
			if(pos > 0) {
				s.assign(&str[pos]);
				ls.push_back(s);
			}

			break;
		}

		if(ppos == pos)
			break;
		else
			ppos = pos;

		/* now pickup the first mach because it is the full match */
		it = matches.begin();

		/* should never happen, but you never know */
		if((*it).offset < pos) {
			E_WARNING(E_STRLOC ": Unable to correctly calculate length of the match (%i %i)\n", (*it).offset, pos);
			continue;
		}

		s.assign(&str[pos], (*it).offset - pos);
		ls.push_back(s);

		pos = (*it).offset + (*it).length;
	}

	return ls.size();
}

const char* Regex::strerror(void) const {
	E_ASSERT(data != NULL && "Did you run compile() first?");
	return data->error.c_str();
}

EDELIB_NS_END
