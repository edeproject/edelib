/*
 * $Id$
 *
 * Config file reader and writer
 * Part of edelib.
 * Copyright (c) 2005-2007 EDE Authors.
 *
 * This program is licenced under terms of the
 * GNU General Public Licence version 2 or newer.
 * See COPYING for details.
 */

#include <edelib/econfig.h>
#include <edelib/Config.h>
#include <edelib/Debug.h>
#include <edelib/StrUtil.h>
#include <edelib/File.h>
#include <edelib/Nls.h>

#include <stdio.h>   // snprintf
#include <ctype.h>

#include <string.h>  // strchr, strncpy, strlen
#include <stdlib.h>  // free, atoi, atol, atof, getenv

#ifdef USE_NLS
#include <libintl.h> // setlocale
#endif

// max section len
#define ESECT_MAX 128
// max line len
#define ELINE_MAX 5120
// max key len
#define EKEY_MAX  128
// max value len
#define EVAL_MAX  5120

#define COMMENT    '#'
#define SECT_OPEN  '['
#define SECT_CLOSE ']'
#define KV_DELIM   '='

#define EAT_SPACES(ptr) while(isspace(*ptr) && *ptr) ptr++


EDELIB_NAMESPACE {

/* A hash function from Dr.Dobbs Journal.
 * Althought, author is claiming it is collision free,
 * explicit string comparisons should be maded, after
 * matching hash is found
 */
unsigned int do_hash(const char* key, int keylen) {
	unsigned hash ;
	int i;
	for (i = 0, hash = 0; i < keylen ;i++) {
		hash += (long)key[i] ;
		hash += (hash<<10);
		hash ^= (hash>>6) ;
	}
	hash += (hash <<3);
	hash ^= (hash >>11);
	hash += (hash <<15);
	return hash ;
}

/*
 * scan section in [section]
 * returned value is terminated with '\0'
 */
bool scan_section(char* line, char* buff, int buffsz) {
	char* buffp = buff;
	bool status = false;

	str_trimleft(line);

	for (; *line && buffsz > 0; line++) {
		if (*line == SECT_CLOSE) {
			status = true;
			break;
		}
		*buffp++ = *line;
		buffsz--;
	}
	*buffp = '\0';
	str_trimright(buff);
	return status;
}

/*
 * scans key = value part
 * returned values are terminated with '\0'
 */
bool scan_keyvalues(char* line, char* key, char* val, int keysz, int valsz) {
	char* linep = line;

	char* pos = strchr(line, KV_DELIM);
	if (!pos)
		return false;

	int i;
	for (i = 0; (*linep != *pos) && (i < keysz); linep++, i++)
		key[i] = *linep;
	key[i] = '\0';

	// skip '='
	if (*linep == '=')
		linep++;

	for (i = 0; *linep && (*linep != '\n') && i < valsz; linep++, i++)
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
	EASSERT(n != NULL);

	snamelen = strlen(n);
	sname = strdup(n);
	shash = do_hash(sname, snamelen);
}

ConfigSection::~ConfigSection() {
	EntryList::iterator it = entry_list.begin();
	for (; it != entry_list.end(); ++it) {
		ConfigEntry* e = *it;
#ifdef CONFIG_INTERNAL
		printf("deleting: %s\n", e->key);
#endif
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
	EASSERT(key != NULL);
	EASSERT(value != NULL);

	ConfigEntry* e = find_entry(key);
	if (!e) {
		e = new ConfigEntry;
		e->keylen = strlen(key);
		e->valuelen = strlen(value);
		e->key = strdup(key);
		e->value = strdup(value);
		// hash the key
		e->hash = do_hash(e->key, e->keylen);

		EASSERT(e->key != NULL);
		EASSERT(e->value != NULL);
#ifdef CONFIG_INTERNAL
		printf("Adding: |%s| = |%s| hash = %i section = %s\n", e->key, e->value, e->hash, sname);
#endif
		entry_list.push_back(e);
	} else {
#ifdef CONFIG_INTERNAL
		printf("!!! found duplicate for %s\n", e->key);
#endif
		free(e->value);
		e->valuelen = strlen(value);
		e->value = strdup(value);
		EASSERT(e->value != NULL);
	}
}

void ConfigSection::remove_entry(char* key) {}

ConfigEntry* ConfigSection::find_entry(const char* key) {
	EASSERT(key != NULL);

	int klen = strlen(key);
	unsigned hh = do_hash(key, klen);
	EntryList::iterator it = entry_list.begin();

	for (; it != entry_list.end(); ++it) {
		ConfigEntry* e = *it;
		if (hh == e->hash && strncmp(e->key, key, e->keylen) == 0)
			return e;
	}
	return NULL;
}

/*
 * Config methods
 */
Config::Config() : errcode(0), linenum(0), sectnum(0), cached(0) {}

Config::~Config() {
	clear();
}

bool Config::load(const char* fname) {
	EASSERT(fname != NULL);

#ifdef CONFIG_USE_STDIO
	FILE *f = fopen(fname, "r");
	if (!f) {
		errcode = CONF_ERR_FILE;
		return false;
	}
#else
	File f;
	if (!f.open(fname)) {
		errcode = CONF_ERR_FILE;
		return false;
	}
#endif

	// set default return values
	errcode = CONF_SUCCESS;
	bool status = true;

	// we must have at least one section
	bool sect_found = false;

	char buff[ELINE_MAX];
	char section[ESECT_MAX];
	char keybuff[EKEY_MAX];
	char valbuff[EVAL_MAX];

	char *buffp;
	ConfigSection* tsect = NULL;

#if CONFIG_USE_STDIO
	while (fgets(buff, sizeof(buff)-1, f))
#else
	while (f.readline(buff, sizeof(buff)-1) >= 0)
#endif
	{
		++linenum;

		buffp = buff;

		EAT_SPACES(buffp);
		// comment or empty line
		if (*buffp == COMMENT || *buffp == '\0')
			continue;

		// we found an section
		if (*buffp == SECT_OPEN) {
			sect_found = true;
			buffp++;
			if (!scan_section(buffp, section, sizeof(section))) {
				errcode = CONF_ERR_BAD;
				status = false;
				break;
			} else {
				/*
				 * first check if section exists, or create 
				 * it if not
				 */
				tsect = find_section(section);
				if (!tsect) {
#ifdef CONFIG_INTERNAL
					printf("---------------> adding section %s\n", section);
#endif
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

			if (!scan_keyvalues(buffp, keybuff, valbuff, EKEY_MAX, EVAL_MAX)) {
				errcode = CONF_ERR_BAD;
				status = false;
				break;
			}

			EASSERT(tsect != NULL && "Entry without a section ?!");
			tsect->add_entry(keybuff, valbuff);
		}
	}
#ifdef CONFIG_USE_STDIO
	fclose(f);
#endif
	return status;
}

bool Config::save(const char* fname) {
	EASSERT(fname != NULL);
#ifdef CONFIG_USE_STDIO
	FILE* f = fopen(fname, "w");
	if (!f) {
		errcode = CONF_ERR_FILE;
		return false;
	}
#else
	File f;
	if (!f.open(fname, FIO_WRITE)) {
		errcode = CONF_ERR_FILE;
		return false;
	}
#endif

	SectionList::iterator sit = section_list.begin();
	EntryList::iterator eit;

#ifdef CONFIG_USE_STDIO
	for (; sit != section_list.end(); ++sit) {
		fprintf(f, "[%s]\n", (*sit)->sname);

		for (eit = (*sit)->entry_list.begin(); eit != (*sit)->entry_list.end(); ++eit)
			fprintf(f, "   %s = %s\n", (*eit)->key, (*eit)->value);

		fprintf(f, "\n");
	}
	fclose(f);
#else
	for (; sit != section_list.end(); ++sit) {
		f.printf("[%s]\n", (*sit)->sname);

		for (eit = (*sit)->entry_list.begin(); eit != (*sit)->entry_list.end(); ++eit)
			f.printf("   %s = %s\n", (*eit)->key, (*eit)->value);

		f.printf("\n");
	}
#endif
	return true;
}

/*
 * Create section if not exist, or if exist
 * return that one.
 */
ConfigSection* Config::add_section(const char* section) {
	EASSERT(section != NULL);

	ConfigSection* sc = find_section(section);
	if (!sc) {
#ifdef CONFIG_INTERNAL
		printf("---------------> adding section %s\n", section);
#endif
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
	EASSERT(section != NULL);
	int slen = strlen(section);
	unsigned hh = do_hash(section, slen);

	// check if we have cached section
	if (cached && cached->shash == hh && (strncmp(cached->sname, section, cached->snamelen) == 0)) {
#ifdef CONFIG_INTERNAL
		printf("Found %s cached\n", cached->sname);
#endif
		return cached;
	}

	SectionList::iterator it = section_list.begin();
	for (; it != section_list.end(); ++it) {
		ConfigSection *cs = *it;
		if (cs->shash == hh && (strncmp(cs->sname, section, cs->snamelen) == 0)) {
#ifdef CONFIG_INTERNAL
			printf("XXX found: %s\n", cs->sname);
#endif
			cached = cs;
			return cs;
		}
	}
	return NULL;
}

void Config::clear(void) {
	SectionList::iterator it = section_list.begin();
	for (; it != section_list.end(); ++it)
		delete *it;
	section_list.clear();
}

bool Config::exist(const char* section) {
	return (find_section(section) != NULL);
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
	//ret = (bool)atoi(value);

	str_tolower((unsigned char*)value);
	int len = strlen(value);

	if (len == 4 && (strncmp(value, "true", 4) == 0))
		ret = true;
	else if (len == 5 && (strncmp(value, "false", 5) == 0))
		ret = false;
	else if (len == 1 && (strncmp(value, "1", 1) == 0))
		ret = true;
	else if (len == 0 && (strncmp(value, "0", 1) == 0))
		ret = false;
	else
		ret = deflt;
	return true;
}

bool Config::get(const char* section, const char* key, double& ret, double deflt) {
	GET_VALUE(section, key, ret, deflt);
	ret = atof(value);
	return true;
}

bool Config::get(const char* section, const char* key, long& ret, long deflt) {
	GET_VALUE(section, key, ret, deflt);
	ret = atol(value);
	return true;
}

bool Config::get(const char* section, const char* key, float& ret, float deflt) {
	GET_VALUE(section, key, ret, deflt);
	ret = atof(value);
	return true;
}

bool Config::get(const char* section, const char* key, char& ret, char deflt) {
	GET_VALUE(section, key, ret, deflt);
	ret = value[0];
	return true;
}

bool Config::get(const char* section, const char* key, char* ret, int size) {
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
	return true;
}

bool Config::get_localized(const char* section, const char* key, char* ret, int size) {
	char* lang;
	/*
	#ifdef USE_NLS
		if((lang = setlocale(LC_MESSAGES, NULL)) == NULL)
	#endif
	*/
	lang = getenv("LANG");

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

	char key_buff[128];

	/*
	 * Config class can accept Name[xxx] names as
	 * keys, so we use it here; first construct
	 * a key name, and try to find it
	 */
	snprintf(key_buff, sizeof(key_buff), "%s[%s]", key, lang);
	bool found = false;

	// first try to find it with full data
	ConfigEntry* ce = cs->find_entry(key_buff);
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

				//printf("have code: %s lang: %s sz: %i\n", code, lang, sz);
				snprintf(key_buff, sizeof(key_buff), "%s[%s]", key, code);
				//printf("trying with: %c: %s\n", delim[i], key_buff);
				delete [] code;

				ce = cs->find_entry(key_buff);
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
		return true;
	} else
		errcode = CONF_ERR_KEY;
	return false;
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

	char key_buff[128];
	snprintf(key_buff, sizeof(key_buff), "%s[%s]", key, lang_val);
	set(section, key_buff, val);
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
	// TODO: set locale to C
	char tmp[32];
	snprintf(tmp, sizeof(tmp)-1, "%g", value);
	// TODO: restore locale from C
	sc->add_entry(key, tmp);
}

void Config::set(const char* section, const char* key, double value) {
	ConfigSection* sc = add_section(section);
	// TODO: set locale to C
	char tmp[32];
	snprintf(tmp, sizeof(tmp)-1, "%g", value);
	// TODO: restore locale from C
	sc->add_entry(key, tmp);
}

} // EDELIB_NAMESPACE
