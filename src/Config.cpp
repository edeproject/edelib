/*
 * $Id$
 *
 * Config file reader and writer
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

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <edelib/Config.h>
#include <edelib/TempFile.h>
#include <edelib/Debug.h>
#include <edelib/StrUtil.h>
#include <edelib/Nls.h>

#define ESECT_MAX         128 /* max section len */
#define EKEY_MAX          128 /* max key len */
#define ELINE_SIZE_START 1024 /* starting size for line/value, can grow */

#define COMMENT    '#'
#define SECT_OPEN  '['
#define SECT_CLOSE ']'
#define KV_DELIM   '='

#define EAT_SPACES(ptr) while(*ptr && isspace(*ptr)) ptr++

EDELIB_NS_BEGIN

struct ConfigEntry {
	char* key;
	char* value;
	unsigned int keylen;
	unsigned int valuelen;
	unsigned int hash;
};

class ConfigSection {
private:
	friend class Config;

	char*  sname;
	size_t snamelen;
	unsigned shash;

	EntryList entry_list;

	ConfigSection(const ConfigSection&);
	ConfigSection& operator=(ConfigSection&);

	void add_entry(const char* key, const char* value);
	void remove_entry(const char* key);
	ConfigEntry* find_entry(const char* key);

public:
	ConfigSection(const char* n);
	~ConfigSection();
};

/*
 * Similar to fgets, but will expand buffer as needed. Actually
 * this is the same as getline(), but it is not used since is glibc 
 * extension and is very non-portable.
 * Return number of read characters or -1 if error/EOF occured.
 *
 * Notice that given buffer can be NULL; in that case it will
 * allocate needed memory size (see man getline). Also, contrary
 * to the glibc getline(), allocated data must be cleared with delete[],
 * not free().
 */
int config_getline(char** buf, int* len, FILE* f) {
	if(!buf || !len)
		return -1;

	if(!*buf) *len = 0;

	int i = 0;
	int c;

	while(1) {
		c = fgetc(f);
		if(i >= *len) {
			// do not multiply since *len can be 0
			int tmp = *len + 100;

			char* new_buf = new char[tmp];
			strncpy(new_buf, *buf, *len);

			if(*buf) delete [] *buf;
			*buf = new_buf;
			*len = tmp;
		}

		if(c == EOF) {
			(*buf)[i] = '\0';
			return -1;
		}

		(*buf)[i] = c;
		/*
		 * counter is increased here so getline() is fully emulated;
		 * if this is done in for loop, '\n' chars would not be recorded
		 * in returned buffer
		 */
		i++;

		if(c == '\n')
			break;
	}

	(*buf)[i] = '\0';
	return i;
}

/*
 * scan section in [section]
 * returned value is terminated with '\0'
 */
static bool scan_section(char* line, char* buf, int bufsz) {
	char* bufp = buf;
	bool status = false;

	str_trimleft(line);

	for(; *line && bufsz > 0; line++) {
		if(*line == SECT_CLOSE) {
			status = true;
			break;
		}

		*bufp++ = *line;
		bufsz--;
	}

	*bufp = '\0';
	str_trimright(buf);
	return status;
}

/*
 * scans key = value part
 * returned values are terminated with '\0'
 */
static bool scan_keyvalues(char* line, char* key, char* val, int linesz, int keysz, int valsz) {
	char* linep = line;

	char* pos = strchr(line, KV_DELIM);
	if (!pos)
		return false;

	int i;
	for (i = 0; (*linep != *pos) && (i < keysz); linep++, i++)
		key[i] = *linep;
	key[i] = '\0';

	// skip KV_DELIM
	if (*linep == KV_DELIM)
		linep++;

	for (i = 0; *linep && (*linep != '\n') && (i < valsz) && (i < linesz); linep++, i++)
		val[i] = *linep;
	val[i] = '\0';

	// trim spaces
	str_trim(key);
	str_trim(val);

	return true;
}

/*
 * ConfigSection methods
 */
ConfigSection::ConfigSection(const char* n) {
	E_ASSERT(n != NULL);

	snamelen = strlen(n);
	sname = strdup(n);
	shash = str_hash(sname, snamelen);
}

ConfigSection::~ConfigSection() {
	EntryListIter it = entry_list.begin();
	for (; it != entry_list.end(); ++it) {
		ConfigEntry* e = *it;
		free(e->key);
		free(e->value);
		delete e;
		e = NULL;
	}

	free(sname);
}

/*
 * Add new key/value, but first check
 * do we alread have and key, so if
 * we do, just update value
 */
void ConfigSection::add_entry(const char* key, const char* value) {
	E_ASSERT(key != NULL);
	E_ASSERT(value != NULL);

	ConfigEntry* e = find_entry(key);
	if (!e) {
		e = new ConfigEntry;
		e->keylen   = strlen(key);
		e->valuelen = strlen(value);
		e->key      = strdup(key);
		e->value    = strdup(value);
		e->hash     = str_hash(e->key, e->keylen);

		E_ASSERT(e->key != NULL);
		E_ASSERT(e->value != NULL);

		entry_list.push_back(e);
	} else {
		free(e->value);
		e->valuelen = strlen(value);
		e->value    = strdup(value);

		E_ASSERT(e->value != NULL);
	}
}

void ConfigSection::remove_entry(const char* key) {
	E_ASSERT(key != NULL);

	unsigned int hh = str_hash(key);
	EntryListIter it = entry_list.begin();

	for(; it != entry_list.end(); ++it) {
		ConfigEntry* e = *it;
		if(hh == e->hash && strncmp(e->key, key, e->keylen) == 0)
			entry_list.erase(it);
	}
}

ConfigEntry* ConfigSection::find_entry(const char* key) {
	E_ASSERT(key != NULL);

	unsigned int hh = str_hash(key);
	EntryListIter it = entry_list.begin(), it_end = entry_list.end();

	for (; it != it_end; ++it) {
		ConfigEntry* e = *it;
		if (hh == e->hash && strncmp(e->key, key, e->keylen) == 0)
			return e;
	}
	return NULL;
}

/* Config methods */
Config::Config() : errcode(0), linenum(0), sectnum(0), cached(0) {}

bool Config::load(const char* fname) {
	E_ASSERT(fname != NULL);

	clear();

	FILE *f = fopen(fname, "r");
	if (!f) {
		errcode = CONF_ERR_FILE;
		return false;
	}

	// set default return values
	errcode = CONF_SUCCESS;
	bool status = true;

	// we must have at least one section
	bool sect_found = false;

	// use fixed sizes for sections and keys
	char section[ESECT_MAX];
	char keybuf[EKEY_MAX];

	// line and value can grow
	int buflen = ELINE_SIZE_START;
	char* buf = new char[buflen];

	// use the same size as for line
	int valbuflen = buflen;
	char* valbuf = new char[valbuflen];

	char *bufp;
	ConfigSection* tsect = NULL;

	while(config_getline(&buf, &buflen, f) != -1) {
		++linenum;

		bufp = buf;
		EAT_SPACES(bufp);

		// comment or empty line
		if (*bufp == COMMENT || *bufp == '\0')
			continue;

		// we found an section
		if (*bufp == SECT_OPEN) {
			sect_found = true;
			bufp++;
			if (!scan_section(bufp, section, sizeof(section))) {
				errcode = CONF_ERR_BAD;
				status = false;
				break;
			} else {
				// first check if section exists, or create if not
				tsect = find_section(section);
				if (!tsect) {
					++sectnum;
					tsect = new ConfigSection(section);
					section_list.push_back(tsect);
				}
			}
		}
		// data part
		else {
			// file without sections
			if (!sect_found) {
				errcode = CONF_ERR_SECTION;
				status = false;
				break;
			}

			/*
			 * check if size of valbuf is less than buflen;
			 * in that case make it size as buflen (better would be to use 
			 * buflen - EKEY_MAX - '=' - <spaces>, but that would complicate thing,
			 * also more size does not hurts :P)
			 */
			if(valbuflen < buflen) {
				valbuflen = buflen;
				delete [] valbuf;
				valbuf = new char[valbuflen];
			}

			if (!scan_keyvalues(bufp, keybuf, valbuf, buflen, EKEY_MAX, valbuflen)) {
				errcode = CONF_ERR_BAD;
				status = false;
				break;
			}

			E_ASSERT(tsect != NULL && "Entry without a section ?!");
			tsect->add_entry(keybuf, valbuf);
		}
	}

	fclose(f);
	delete [] buf;
	delete [] valbuf;

	return status;
}

bool Config::save(const char* fname) {
	E_ASSERT(fname != NULL);

	TempFile t;
	if(!t.create(".etmp.XXXXXX")) {
		errcode = CONF_ERR_FILE;
		return false;
	}

	/* so we could explicitly handle our options */
	t.set_no_close(true);
	t.set_auto_delete(false);

	FILE *f = t.fstream();

	SectionListIter sit = section_list.begin(), sit_end = section_list.end();
	unsigned int sz = section_list.size();
	EntryListIter eit;

	for (; sit != sit_end; ++sit, --sz) {
		fprintf(f, "[%s]\n", (*sit)->sname);

		for (eit = (*sit)->entry_list.begin(); eit != (*sit)->entry_list.end(); ++eit)
			fprintf(f, "%s=%s\n", (*eit)->key, (*eit)->value);

		/* prevent unneeded newline at the end of file */
		if(sz != 1)
			fprintf(f, "\n");
	}

	/* explicitly flush */
	fflush(f);
	t.close();

	E_ASSERT(t.name() && "Temporary name NULL. Report this as bug");

	if(rename(t.name(), fname) != 0) {
		E_WARNING("Unable to save to '%s'\n", fname);
		return false;
	} 

	chmod(fname, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
	return true;
}

/*
 * Create section if not exist, or if exist
 * return that one.
 */
ConfigSection* Config::add_section(const char* section) {
	E_ASSERT(section != NULL);

	ConfigSection* sc = find_section(section);
	if (!sc) {
		++sectnum;
		sc = new ConfigSection(section);
		section_list.push_back(sc);
	}
	return sc;
}

/*
 * Scans throught the section list, and if found
 * return it. Otherwise, return NULL.
 */
ConfigSection* Config::find_section(const char* section) {
	E_ASSERT(section != NULL);

	unsigned int hh = str_hash(section);

	// check if we have cached section
	if (cached && cached->shash == hh && (strncmp(cached->sname, section, cached->snamelen) == 0))
		return cached;

	SectionListIter it = section_list.begin(), it_end = section_list.end();
	for (; it != it_end; ++it) {
		ConfigSection *cs = *it;
		if (cs->shash == hh && (strncmp(cs->sname, section, cs->snamelen) == 0)) {
			cached = cs;
			return cs;
		}
	}
	return NULL;
}

void Config::clear(void) {
	SectionListIter it = section_list.begin(), it_end = section_list.end();
	for (; it != it_end; ++it)
		delete *it;
	section_list.clear();

	errcode = 0;
	linenum = 0;
	sectnum = 0;
	cached = 0;
}

bool Config::exist(const char* section) {
	return (find_section(section) != NULL);
}

bool Config::key_exist(const char* section, const char* key) {
	ConfigSection* cs = find_section(section);
	if(!cs)
		return false;
	return (cs->find_entry(key) != NULL);
}

unsigned int Config::num_sections(void) {
	return sectnum;
}

const char* Config::strerror(int code) {
	switch (code) {
	case CONF_SUCCESS:
		return _("Successful completion");
	case CONF_ERR_FILE:
		return _("Could not access config file");
	case CONF_ERR_BAD:
		return _("Malformed config file");
	case CONF_ERR_SECTION:
		return _("Config file section not found");
	case CONF_ERR_KEY:
		return _("Key not found in section");
	case CONF_ERR_MEMORY:
		return _("Could not allocate memory");
	case CONF_ERR_NOVALUE:
		return _("Invalid value associated with the key");
	default:
		return _("Unknown error");
	}
}

const char* Config::strerror(void) {
	return strerror(errcode);
}

unsigned int Config::line(void) {
	return linenum;
}

int Config::error(void) {
	return errcode;
}

#define GET_VALUE(sect, key, ret, dflt) \
ConfigSection* cs = find_section(section); \
if(!cs) {\
	errcode = CONF_ERR_SECTION; \
	ret = dflt; \
	return false; \
}\
ConfigEntry* ce = cs->find_entry(key); \
if(!ce) {\
	errcode = CONF_ERR_KEY; \
	ret = dflt; \
	return false; \
}\
char* value = ce->value

bool Config::get(const char* section, const char* key, int& ret, int deflt) {
	GET_VALUE(section, key, ret, deflt);
	ret = atoi(value);
	return true;
}

bool Config::get(const char* section, const char* key, bool& ret, bool deflt) {
	GET_VALUE(section, key, ret, deflt);

	str_tolower((unsigned char*)value);
	int len = strlen(value);

	if (len == 4 && (strncmp(value, "true", 4) == 0))
		ret = true;
	else if (len == 5 && (strncmp(value, "false", 5) == 0))
		ret = false;
	else if (len == 1 && (strncmp(value, "1", 1) == 0))
		ret = true;
	else if (len == 1 && (strncmp(value, "0", 1) == 0))
		ret = false;
	else
		ret = deflt;
	return true;
}

bool Config::get(const char* section, const char* key, double& ret, double deflt) {
	GET_VALUE(section, key, ret, deflt);
	char* locale = nls_locale_to_c();
	ret = atof(value);
	nls_locale_from_c(locale);
	return true;
}

bool Config::get(const char* section, const char* key, long& ret, long deflt) {
	GET_VALUE(section, key, ret, deflt);
	ret = atol(value);
	return true;
}

bool Config::get(const char* section, const char* key, float& ret, float deflt) {
	GET_VALUE(section, key, ret, deflt);
	char* locale = nls_locale_to_c();
	ret = atof(value);
	nls_locale_from_c(locale);
	return true;
}

bool Config::get(const char* section, const char* key, char& ret, char deflt) {
	GET_VALUE(section, key, ret, deflt);
	ret = value[0];
	return true;
}

bool Config::get(const char* section, const char* key, char* ret, unsigned int size) {
	ConfigSection* cs = find_section(section);
	if (!cs) {
		errcode = CONF_ERR_SECTION;
		return false;
	}
	ConfigEntry* ce = cs->find_entry(key);
	if (!ce) {
		errcode = CONF_ERR_KEY;
		return false;
	}
	char* value = ce->value;
	strncpy(ret, value, size);

	// again, strncpy does not terminate string if size is less that actual
	if(ce->valuelen > size)
		ret[size-1] = '\0';

	return true;
}

bool Config::get_localized(const char* section, const char* key, char* ret, unsigned int size) {
	char* lang = getenv("LANG");

	// fallback
	if (!lang)
		return get(section, key, ret, size);

	// do not use default locales
	if (lang[0] == 'C' || (strncmp(lang, "en_US", 5) == 0))
		return get(section, key, ret, size);

	ConfigSection* cs = find_section(section);
	if (!cs) {
		errcode = CONF_ERR_SECTION;
		return false;
	}

	char key_buf[128];

	/*
	 * Config class can accept Name[xxx] names as
	 * keys, so we use it here; first construct
	 * a key name, and try to find it
	 */
	snprintf(key_buf, sizeof(key_buf), "%s[%s]", key, lang);
	bool found = false;

	// first try to find it with full data
	ConfigEntry* ce = cs->find_entry(key_buf);
	if (ce)
		found = true;
	else {
		/*
		 * We will try in this order:
		 * 1. lc_CC@qualifier.encoding
		 * 2. lc_CC@qualifier
		 * 3. lc_CC (language code with country code)
		 * 4. lc
		 */
		char delim[] = {'.', '@', '_'};
		char* p;
		char* code;
		for (int i = 0; i < 3; i++) {
			p = strchr(lang, delim[i]);
			if (p != NULL) {
				int sz = p - lang;
				code = new char[sz+1];
				strncpy(code, lang, sz);
				// damint strncpy does not add this
				code[sz] = '\0';

				snprintf(key_buf, sizeof(key_buf), "%s[%s]", key, code);
				delete [] code;

				ce = cs->find_entry(key_buf);
				if (ce) {
					found = true;
					break;
				}
			}
		}
	}

	if (found) {
		char* value = ce->value;
		strncpy(ret, value, size);
		ret[size-1] = '\0';
		return true;
	} else
		errcode = CONF_ERR_KEY;
	return false;
}

bool Config::get_allocated(const char* section, const char* key, char** ret, unsigned int& retsize) {
	retsize = 0;

	ConfigSection* cs = find_section(section);
	if (!cs) {
		errcode = CONF_ERR_SECTION;
		return false;
	}

	ConfigEntry* ce = cs->find_entry(key);
	if (!ce) {
		errcode = CONF_ERR_KEY;
		return false;
	}

	char* value = ce->value;
	retsize = ce->valuelen;

	*ret = new char[retsize + 1];
	strncpy(*ret, value, retsize);
	// terminate, since ce->valuelen does not contain terminating char
	char* p = *ret;
	p[retsize] = '\0';

	return true;
}

void Config::set(const char* section, const char* key, const char* value) {
	ConfigSection* sc = add_section(section);
	// it will create entry, if needed
	sc->add_entry(key, value);
}

void Config::set(const char* section, const char* key, char* value) {
	set(section, key, (const char*)value);
}

void Config::set_localized(const char* section, const char* key, const char* val) {
	char* lang = getenv("LANG");

	// fallback
	if(!lang) {
		set(section, key, val);
		return;
	}

	// do not use default locales
	if (lang[0] == 'C' || (strncmp(lang, "en_US", 5) == 0)) {
		set(section, key, val);
		return;
	}

	// we will use only first two chars
	char lang_val[3];
	if(strlen(lang) > 2) {
		lang_val[0] = lang[0];
		lang_val[1] = lang[1];
		lang_val[2] = '\0';
	} else {
		// wrong code
		set(section, key, val);
		return;
	}

	char key_buf[128];
	snprintf(key_buf, sizeof(key_buf), "%s[%s]", key, lang_val);
	set(section, key_buf, val);
}

void Config::set_localized(const char* section, const char* key, char* val) {
	set_localized(section, key, (const char*)val);
}

void Config::set(const char* section, const char* key, bool value) {
	ConfigSection* sc = add_section(section);
	const char* v = ((value) ? "1" : "0");
	sc->add_entry(key, v);
}

void Config::set(const char* section, const char* key, int value) {
	ConfigSection* sc = add_section(section);
	char tmp[128];
	snprintf(tmp, sizeof(tmp)-1, "%d", value);
	sc->add_entry(key, tmp);
}

void Config::set(const char* section, const char* key, long value) {
	ConfigSection* sc = add_section(section);
	char tmp[128];
	snprintf(tmp, sizeof(tmp)-1, "%ld", value);
	sc->add_entry(key, tmp);
}

void Config::set(const char* section, const char* key, float value) {
	ConfigSection* sc = add_section(section);
	char* locale = nls_locale_to_c();

	char tmp[32];
	snprintf(tmp, sizeof(tmp)-1, "%g", value);

	nls_locale_from_c(locale);
	sc->add_entry(key, tmp);
}

void Config::set(const char* section, const char* key, double value) {
	ConfigSection* sc = add_section(section);
	char* locale = nls_locale_to_c();

	char tmp[32];
	snprintf(tmp, sizeof(tmp)-1, "%g", value);

	nls_locale_from_c(locale);
	sc->add_entry(key, tmp);
}

EDELIB_NS_END
