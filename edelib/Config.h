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

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "econfig.h"
#include "List.h"

EDELIB_NAMESPACE {

enum ConfigErrors {
	CONF_SUCCESS = 0,   ///< successful operation
	CONF_ERR_FILE,      ///< trouble accessing config file or directory
	CONF_ERR_BAD,       ///< malformed file
	CONF_ERR_SECTION,   ///< requested section was not found
	CONF_ERR_KEY,       ///< requested key was not found
	CONF_ERR_MEMORY,    ///< memory allocation error
	CONF_ERR_NOVALUE    ///< key found, but invalid value associated with it
};

#ifndef SKIP_DOCS
struct ConfigEntry {
	char* key;
	char* value;
	unsigned int keylen;
	unsigned int valuelen;
	unsigned int hash;
};
#endif

class Config;
class ConfigSection;

#ifndef SKIP_DOCS
typedef list<ConfigEntry*> EntryList;
typedef list<ConfigSection*> SectionList;
#endif

#ifndef SKIP_DOCS
/**
 * \class ConfigSection
 * Section in config file.
 *
 * Section is a field in config file, containing
 * grouped data. 
 */
class EDELIB_API ConfigSection {
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
#endif


/**
 * \class Config
 * \brief A config file reader.
 *
 * This class is aimed for reading and storing classic INI files
 * in form:
 * \code
 *  # this is a comment
 *  [section1]
 *   item = value
 *
 *  [section2]
 *   item = value
 * \endcode
 * Section names can be words or multiple words separated by spaces.
 * All leading and following spaces will be trimmed, like:
 * \code
 *  [ some long section ] -> "some long section"
 * \endcode
 *
 * Duplicate sections will be merged in one (later when used as output),
 * but in case of duplicate key lines, only last will be considered.
 * Like in this sample:
 * \code
 *  [my section]
 *    idea = bad   <- ignored
 *    idea = good
 * \endcode
 *
 * In above case, using 
 * \code
 *   get("my section", "idea", &val, valsz) 
 * \endcode
 * will set val to "good".
 *
 * During reading phase, file is checked is malformed and if that
 * was true, CONF_ERR_BAD will be returned as error code. Target line
 * can be inspected with line() function.
 *
 * Althought there are various INI specifications, currently supported
 * one is very basic, without subsections support, like:
 * \code
 *  [parent]
 *    item = value
 *  [parent/child]
 *    item = value
 * \endcode
 *
 * Later option can be checked like:
 * \code
 *   get("parent/child", "item", &val, valsz);
 * \endcode
 */
class EDELIB_API Config {
	private:
		unsigned int errcode;
		unsigned int linenum;
		unsigned int sectnum;
		ConfigSection* cached;

		SectionList section_list;

		ConfigSection* add_section(const char* section);
		ConfigSection* find_section(const char* section);

		Config(const Config&);
		Config& operator=(Config&);

	public:
		Config();
		~Config();
		/**
		 * Access custom file on file system.
		 *
		 * \return true if file reading was ok, otherwise false.
		 * \param fname path to config file.
		 */
		bool load(const char* fname);

		/**
		 * Save content to the file.
		 *
		 * \return true if file saving was ok, otherwise false.
		 * \param fname path to store config file.
		 */
		bool save(const char* fname);

		/**
		 * Validate is previous operation was succesfull.
		 *
		 * This is an shorthand for various error() checks, so
		 * it can be used:
		 * \code
		 *   Config c;
		 *   c.load("/no/file");
		 *   if(!c) // do something smart
		 * \endcode
		 *
		 * \return true if everything is fine, or false
		 */
		operator bool(void) const   { return ((errcode == CONF_SUCCESS) ? 1 : 0); }

		/**
		 * Clear all internal data.
		 */
		void clear(void);

		/**
		 * Get char* value from named section.
		 *
		 * \return true if value exist
		 * \param section name of target section
		 * \param key name of target key
		 * \param ret buffer to copy returned value
		 * \param size size of buffer
		 */
		bool get(const char* section, const char* key, char* ret, unsigned int size);

		/**
		 * Get localized character data. This method behaves the same as get()
		 * with character data, except it will search first keys with locale
		 * tags, which coresponds system locale settings. This keys looks like:
		 * \code
		 *   # localized value
		 *   Key[de] = localized value in german
		 *
		 *   # unlocalized value
		 *   Key = some value
		 * \endcode
		 *
		 * Function will try to find out current locale settings and if fails,
		 * of specific key with locale tag does not exist, it will try with
		 * ordinary unlocalized value.
		 *
		 * \return true if value exist
		 * \param section name of target section
		 * \param key name of target key
		 * \param ret buffer to copy returned value
		 * \param size size of buffer
		 */
		bool get_localized(const char* section, const char* key, char* ret, unsigned int size);

		/**
		 * Get bool value from named section.
		 *
		 * \return true if value exist
		 * \param section name of target section
		 * \param key name of target key
		 * \param ret where to place returned value
		 * \param dfl default value, in case real value not found
		 */
		bool get(const char* section, const char* key, bool& ret, bool dfl = false);

		/**
		 * Get int value from named section.
		 *
		 * \return true if value exist
		 * \param section name of target section
		 * \param key name of target key
		 * \param ret where to place returned value
		 * \param dfl default value, in case real value not found
		 */
		bool get(const char* section, const char* key, int& ret, int dfl = 0);

		/**
		 * Get float value from named section.
		 *
		 * \return true if value exist
		 * \param section name of target section
		 * \param key name of target key
		 * \param ret where to place returned value
		 * \param dfl default value, in case real value not found
		 */
		bool get(const char* section, const char* key, float& ret, float dfl = 0);

		/**
		 * Get long value from named section.
		 *
		 * \return true if value exist
		 * \param section name of target section
		 * \param key name of target key
		 * \param ret where to place returned value
		 * \param dfl default value, in case real value not found
		 */
		bool get(const char* section, const char* key, long& ret, long dfl = 0);

		/**
		 * Get double value from named section.
		 *
		 * \return true if value exist
		 * \param section name of target section
		 * \param key name of target key
		 * \param ret where to place returned value
		 * \param dfl default value, in case real value not found
		 */
		bool get(const char* section, const char* key, double& ret, double dfl = 0);

		/**
		 * Get char value from named section.
		 *
		 * \return true if value exist
		 * \param section name of target section
		 * \param key name of target key
		 * \param ret where to place returned value
		 * \param dfl default value, in case real value not found
		 */
		bool get(const char* section, const char* key, char& ret, char dfl = 0);

		/**
		 * Set char* value in named section. If section, or key does not
		 * exists, they will be created.
		 *
		 * \param section name of target section
		 * \param key name of target key
		 * \param val value to store within key
		 */
		void set(const char* section, const char* key, char* val);

		/**
		 * Set const char* value in named section. If section, or key does not
		 * exists, they will be created.
		 *
		 * \param section name of target section
		 * \param key name of target key
		 * \param val value to store within key
		 */
		void set(const char* section, const char* key, const char* val);

		/**
		 * Set char* value for localized named section. Description for get_localized()
		 * applies here too.
		 *
		 * \param section name of target section
		 * \param key name of target key
		 * \param val value to store within key
		 */
		void set_localized(const char* section, const char* key, char* val);

		/**
		 * Set const char* value for localized named section. Description for get_localized()
		 * applies here too.
		 *
		 * \param section name of target section
		 * \param key name of target key
		 * \param val value to store within key
		 */
		void set_localized(const char* section, const char* key, const char* val);

		/**
		 * Set bool value in named section. If section, or key does not
		 * exists, they will be created.
		 *
		 * \param section name of target section
		 * \param key name of target key
		 * \param val value to store within key
		 */
		void set(const char* section, const char* key, bool val);

		/**
		 * Set int value in named section. If section, or key does not
		 * exists, they will be created.
		 *
		 * \param section name of target section
		 * \param key name of target key
		 * \param val value to store within key
		 */
		void set(const char* section, const char* key, int val);

		/**
		 * Set long value in named section. If section, or key does not
		 * exists, they will be created.
		 *
		 * \param section name of target section
		 * \param key name of target key
		 * \param val value to store within key
		 */
		void set(const char* section, const char* key, long val);

		/**
		 * Set float value in named section. If section, or key does not
		 * exists, they will be created.
		 *
		 * \param section name of target section
		 * \param key name of target key
		 * \param val value to store within key
		 */
		void set(const char* section, const char* key, float val);

		/**
		 * Set double value in named section. If section, or key does not
		 * exists, they will be created.
		 *
		 * \param section name of target section
		 * \param key name of target key
		 * \param val value to store within key
		 */
		void set(const char* section, const char* key, double val);

		/**
		 * Check if section exist.
		 *
		 * \return true if exist, or false if not.
		 */
		bool exist(const char* section);

		/**
		 * Return number of sections.
		 *
		 * \return number of sections.
		 */
		unsigned int num_sections(void);

		/**
		 * Get current line number.
		 *
		 * This function can be used if file is malformed
		 * \return problematic line
		 */
		unsigned int line(void);

		/**
		 * Get error code.
		 *
		 * This can be used to inspect why file can't be read.
		 * \return one of ConfigErrors values
		 */
		int  error(void);

		/**
		 * Get error in string form.
		 * \return current error in string form
		 */
		const char* strerror(void);

		/**
		 * Interpret parameter and return associated error code.
		 * \return string name for error code
		 * \param code one of ConfigErrors values
		 */
		const char* strerror(int code);
};

/*
 * These functions are for unit test only.
 * They should not be used in application code.
 */
#ifndef SKIP_DOCS
class File;

int config_getline(char** buff, int* len, FILE* f);
int config_getline(char** buff, int* len, File* f);
#endif

}

#endif // __CONFIG_H__
