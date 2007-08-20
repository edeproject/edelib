/*
 * $Id$
 *
 * Regex class
 * Part of edelib.
 * Copyright (c) 2000-2007 EDE Authors.
 *
 * This program is licensed under terms of the
 * GNU General Public License version 2 or newer.
 * See COPYING for details.
 */

#include <edelib/Regex.h>
#include <edelib/Debug.h>
#include <edelib/Nls.h>

extern "C" {
#include "rx/regex.h"
}

#include <string.h> // strlen
#include <stdlib.h> // free

#define CHECK_MODE(all, mode) ((all & mode) == mode)

EDELIB_NS_BEGIN

struct RegexData { 
	bool loaded;
	re_registers* reg; // allocated in search()
	regex_t rpattern;
};

Regex::Regex() : errcode(RX_EMPTY), data(0) { }

Regex::~Regex() {
	if(data) {
		clear();
		clear_regs();
		delete data;
	}
}

void Regex::clear(void) {
	EASSERT(data != NULL);

	if(data->loaded) {
		regfree(&data->rpattern);
		data->loaded = false;
	}

	errcode = RX_EMPTY;
}

void Regex::clear_regs(void) {
	EASSERT(data != NULL);

	if(data->reg) {
		if(data->reg->start)
			free(data->reg->start);
		if(data->reg->end)
			free(data->reg->end);

		data->reg->start = 0;
		data->reg->end = 0;

		delete data->reg;
		data->reg = 0;
	}
}

bool Regex::compile(const char* pattern, int m) { 
	if(!data) {
		data = new RegexData;
		data->loaded = false;
		data->reg = 0;
	}

	clear();
	clear_regs();

	int mode = 0;
	if(CHECK_MODE(m, RX_BASIC))
		mode |= 0;

	if(CHECK_MODE(m, RX_EXTENDED))
		mode |= REG_EXTENDED;

	if(CHECK_MODE(m, RX_ICASE))
		mode |= REG_ICASE;
	
	if(CHECK_MODE(m, RX_NEWLINE))
		mode |= REG_NEWLINE;

	int ret = regcomp(&data->rpattern, pattern, mode);
	if(ret == 0) {
		data->loaded = true;
		errcode = RX_SUCCESS;
	} else {
		clear();
		map_errcode(ret);
	}

	return data->loaded;
}

bool Regex::match(const char* str) { 
	EASSERT(data != NULL && "compile() must be called first");
	EASSERT(data->loaded == true && "compile() must be successfull");
	EASSERT(str != NULL);

	int ret = regexec(&data->rpattern, str, 0, 0, 0);
	return (ret == 0);
}

int Regex::search(const char* str, int len, int& matchlen, int start) {
	EASSERT(data != NULL && "compile() must be called first");
	EASSERT(data->loaded == true && "compile() must be successfull");
	EASSERT(str != NULL);

	int range, spos;
	if(start >= 0) {
		spos = start;
		range = len - start;
	} else {
		spos = start + len;
		range = -spos;
	}

	/*
	 * Must use re_registers as pointer since ordinary variable will crash code 
	 * when search() is called multiple times (like in split()). I'm assuming that 
	 * re_search() keeps internal static (sic) code and re-use it through multiple calls
	 *
	 * Also at start zero start/end values because if re_search() fails, it will not
	 * allocate anything so clear_regs() does not segv.
	 */
	if(!data->reg) {
		data->reg = new re_registers;
		data->reg->start = 0;
		data->reg->end = 0;
	}
	
	int p = re_search(&data->rpattern, str, len, spos, range, data->reg);

	if(p >= 0)
		matchlen = data->reg->end[0] - data->reg->start[0];
	else
		matchlen = 0;

	// -2 is re_search() internal error
	if(p == -2)
		errcode = RX_ERR_SEARCH;

	return p;
}

int Regex::search(const char* str, int& matchlen, int start) {
	return search(str, strlen(str), matchlen, start);
}

int Regex::split(const char* str, list<String>& ls) {
	EASSERT(data != NULL && "compile() must be called first");
	EASSERT(data->loaded == true && "compile() must be successfull");
	EASSERT(str != NULL);

	int count, len, mlen, pos, ppos, start;
	count = len = mlen = pos = start = 0;

	/*
	 * This is used to protect loop against nasty expressions
	 * which could put it to infinity (eg. '[a-zA-Z]*' on 'abc 234 abc')
	 * -10 is random value since search() return -2 >=
	 */
	ppos = -10;

	len = strlen(str);
	if(!len)
		return 0;

	String ss;
	do {
		pos = search(str, len, mlen, start);
		if(pos < 0)
			break;

		if(ppos == pos)
			break;
		else
			ppos = pos;

		ss.assign(str + pos, mlen);
		ls.push_back(ss);

		start = pos + mlen;
	} while(pos >= 0 && start < len);

	return ls.size();
}

/*
 * This actually should be match() with grouping support.
 * Until implemented, I will keep it here as reference.
 */
#if 0
int Regex::split(const char* str, list<String>& ls) {
	EASSERT(data != NULL && "compile() must be called first");
	EASSERT(data->loaded == true && "compile() must be successfull");
	EASSERT(str != NULL);

	regmatch_t matches[10];

	int ret = regexec(&data->rpattern, str, 10, matches, 0);
	if(ret != 0) 
		return 0;

	int len = strlen(str);
	const char* ptr;

	matches[0].rm_so = -1;
	matches[0].rm_eo = -1;

	// matches[0] is whole matched pattern, so we skip it
	for(int i = 1; i < 10; i++) {
		if(matches[i].rm_so >= 0 && matches[i].rm_eo >= 0 && matches[i].rm_so <= len && 
				matches[i].rm_eo <= len && matches[i].rm_so <= matches[i].rm_eo) {
			int sz = matches[i].rm_eo - matches[i].rm_so;
			ptr = str + matches[i].rm_so;

			String s;
			s.assign(ptr, sz);
			EDEBUG(ESTRLOC ": !!! %s %i\n", s.c_str(), sz);
		}
	}

	return 0;
}
#endif

// map regcomp() return value to errcode
void Regex::map_errcode(int ret) {
	switch(ret) {
		case REG_BADBR:
			errcode = RX_ERR_BADBR;
			break;
		case REG_BADPAT:
			errcode = RX_ERR_BADPAT;
			break;
		case REG_BADRPT:
			errcode = RX_ERR_BADRPT;
			break;
		case REG_EBRACE:
			errcode = RX_ERR_EBRACE;
			break;
		case REG_EBRACK:
			errcode = RX_ERR_EBRACK;
			break;
		case REG_ECOLLATE:
			errcode = RX_ERR_ECOLLATE;
			break;
		case REG_ECTYPE:
			errcode = RX_ERR_ECTYPE;
			break;
		case REG_EEND:
			errcode = RX_ERR_EEND;
			break;
		case REG_EESCAPE:
			errcode = RX_ERR_EESCAPE;
			break;
		case REG_EPAREN:
			errcode = RX_ERR_EPAREN;
			break;
		case REG_ERANGE:
			errcode = RX_ERR_ERANGE;
			break;
		case REG_ESPACE:
			errcode = RX_ERR_ESPACE;
			break;
		case REG_ESUBREG:
			errcode = RX_ERR_ESUBREG;
			break;
		default:
			errcode = RX_ERR_UNKNOWN;
	}
}

const char* Regex::strerror(int code) {
	switch(code) {
		case RX_SUCCESS:
			return _("Successful completion");
		case RX_EMPTY:
			return _("Pattern not compiled");
		case RX_ERR_BADBR:
			return _("Invalid use of back reference operator");
		case RX_ERR_BADPAT:
			return _("Invalid use of pattern operators like group");
		case RX_ERR_BADRPT:
			return _("Invalid use of repetition operators such as '*'");
		case RX_ERR_EBRACE:
			return _("Un-matched brace");
		case RX_ERR_EBRACK:
			return _("Un-mached bracket");
		case RX_ERR_ECOLLATE:
			return _("Invalid collating element");
		case RX_ERR_ECTYPE:
			return _("Unknown character");
		case RX_ERR_EESCAPE:
			return _("Trailing backslash");
		case RX_ERR_EPAREN:
			return _("Un-matched parenthesis");
		case RX_ERR_ERANGE:
			return _("Invalid use of range operator");
		case RX_ERR_ESPACE:
			return _("Out of memory");
		case RX_ERR_ESUBREG:
			return _("Invalid backreference to subexpression");
		case RX_ERR_EEND:
			return _("Non specific error");
		case RX_ERR_SEARCH:
			return _("re_search() internal error");
		case RX_ERR_UNKNOWN:
			// fall to default
		default:
			return _("Unknown error");
	}
}

EDELIB_NS_END

