/*
 * $Id$
 *
 * Resource reader and writer
 * Part of edelib.
 * Copyright (c) 2008 EDE Authors.
 *
 * This program is licensed under terms of the 
 * GNU General Public License version 2 or newer.
 * See COPYING for details.
 */

#include <edelib/Resource.h>
#include <edelib/Config.h>
#include <edelib/Debug.h>
#include <edelib/Util.h>
#include <edelib/File.h>
#include <edelib/Directory.h>

EDELIB_NS_BEGIN

typedef list<String> StrList;
typedef list<String>::iterator StrListIter;

Resource::Resource() : sys_conf(NULL), user_conf(NULL) {
}

Resource::~Resource() {
	clear();
}

bool Resource::load(const char* domain) {
	EASSERT(domain != NULL);

	clear();

	String file = domain;
	file += ".conf";

	// check first in system directories
	StrList dirs;
	if(system_config_dirs(dirs)) {
		StrListIter it = dirs.begin(), it_end = dirs.end();
		const char* p = NULL;

		for(; it != it_end; ++it) {
			*it += E_DIR_SEPARATOR;
			*it += file;

			if(file_exists((*it).c_str())) {
				p = (*it).c_str();
				break;
			}
		}

		// now try to load it
		if(p) {
			sys_conf = new Config;

			if(!sys_conf->load(p)) {
				delete sys_conf;
				sys_conf = NULL;
			}
		}
	}

	dirs.clear();

	// now go to user directory
	String ufile = user_config_dir();
	ufile += E_DIR_SEPARATOR;
	ufile += file;

	if(file_exists(ufile.c_str())) {
		user_conf = new Config;

		if(!user_conf->load(ufile.c_str())) {
			delete user_conf;
			user_conf = NULL;
		}
	}

	return (sys_conf != NULL || user_conf != NULL);
}

bool Resource::save(const char* domain) {
	EASSERT(domain != NULL);

	if(!user_conf)
		return false;

	String ufile = user_config_dir();
	ufile += E_DIR_SEPARATOR;
	ufile += domain;
	ufile += ".conf";

	// Create directory if necessary
	String::size_type loc=0, loc2=0;
	while ((loc2=ufile.find('/',loc+1)) != String::npos) loc=loc2; // find last slash
	if (!dir_exists(ufile.substr(0,loc).c_str()))
		dir_create_with_parents(ufile.substr(0,loc).c_str(), 0700); // default perms

	return user_conf->save(ufile.c_str());
}

void Resource::clear(void) {
	delete sys_conf;
	sys_conf = NULL;

	delete user_conf;
	user_conf = NULL;
}

#define FETCH_STR_RESOURCE(section, key, ret, size, rt, get_func)         \
do {                                                                      \
	if(!sys_conf && !user_conf) {                                         \
		return false;                                                     \
	}                                                                     \
	switch(rt) {                                                          \
		case RES_SYS_ONLY:                                                \
			if(!sys_conf) {                                               \
				return false;                                             \
			}                                                             \
			return sys_conf->get_func(section, key, ret, size);           \
		case RES_USER_ONLY:                                               \
			if(!user_conf) {                                              \
				return false;                                             \
			}                                                             \
			return user_conf->get_func(section, key, ret, size);          \
		case RES_SYS_FIRST:                                               \
			if(sys_conf && sys_conf->get_func(section, key, ret, size))   \
				return true;                                              \
			return user_conf->get_func(section, key, ret, size);          \
		case RES_USER_FIRST:                                              \
			if(user_conf && user_conf->get_func(section, key, ret, size)) \
				return true;                                              \
			return sys_conf->get_func(section, key, ret, size);           \
		default:                                                          \
			EASSERT(0 && "Unknown resource type");                        \
	}                                                                     \
    /* never reached */                                                   \
	return false;                                                         \
} while(0)


bool Resource::get(const char* section, const char* key, char* ret, unsigned int size, ResourceType rt) {
	FETCH_STR_RESOURCE(section, key, ret, size, rt, get);
}

bool Resource::get_localized(const char* section, const char* key, char* ret, unsigned int size, ResourceType rt) {
	FETCH_STR_RESOURCE(section, key, ret, size, rt, get_localized);
}

bool Resource::get_allocated(const char* section, const char* key, char** ret, unsigned int& retsize, ResourceType rt) {
	retsize = 0;
	FETCH_STR_RESOURCE(section, key, ret, retsize, rt, get_allocated);
}

#define FETCH_RESOURCE(section, key, ret, dfl, rt)                  \
do {                                                                \
	if(!sys_conf && !user_conf) {                                   \
		ret = dfl;                                                  \
		return false;                                               \
	}                                                               \
                                                                    \
	switch(rt) {                                                    \
		case RES_SYS_ONLY:                                          \
			if(!sys_conf) {                                         \
				ret = dfl;                                          \
				return false;                                       \
			}                                                       \
			return sys_conf->get(section, key, ret, dfl);           \
		case RES_USER_ONLY:                                         \
			if(!user_conf) {                                        \
				ret = dfl;                                          \
				return false;                                       \
			}                                                       \
			return user_conf->get(section, key, ret, dfl);          \
		case RES_SYS_FIRST:                                         \
			if(sys_conf && sys_conf->get(section, key, ret, dfl))   \
				return true;                                        \
			return user_conf->get(section, key, ret, dfl);          \
		case RES_USER_FIRST:                                        \
			if(user_conf && user_conf->get(section, key, ret, dfl)) \
				return true;                                        \
			return sys_conf->get(section, key, ret, dfl);           \
		default:                                                    \
			EASSERT(0 && "Unknown resource type");                  \
	}                                                               \
    /* never reached */                                             \
	return false;                                                   \
} while(0)

bool Resource::get(const char* section, const char* key, bool& ret, bool dfl, ResourceType rt) {
	FETCH_RESOURCE(section, key, ret, dfl, rt);
}

bool Resource::get(const char* section, const char* key, int& ret, int dfl, ResourceType rt) {
	FETCH_RESOURCE(section, key, ret, dfl, rt);
}

bool Resource::get(const char* section, const char* key, float& ret, float dfl, ResourceType rt) {
	FETCH_RESOURCE(section, key, ret, dfl, rt);
}

bool Resource::get(const char* section, const char* key, long& ret, long dfl, ResourceType rt) {
	FETCH_RESOURCE(section, key, ret, dfl, rt);
}

bool Resource::get(const char* section, const char* key, double& ret, double dfl, ResourceType rt) {
	FETCH_RESOURCE(section, key, ret, dfl, rt);
}

bool Resource::get(const char* section, const char* key, char& ret, char dfl, ResourceType rt) {
	FETCH_RESOURCE(section, key, ret, dfl, rt);
}

#define STORE_RESOURCE(section, key, val, store_func) \
do {                                                  \
	if(!user_conf)                                    \
		user_conf = new Config;                       \
	user_conf->store_func(section, key, val);         \
} while(0)

void Resource::set(const char* section, const char* key, char* val) {
	STORE_RESOURCE(section, key, val, set);
}

void Resource::set(const char* section, const char* key, const char* val) {
	STORE_RESOURCE(section, key, val, set);
}

void Resource::set_localized(const char* section, const char* key, char* val) {
	STORE_RESOURCE(section, key, val, set_localized);
}

void Resource::set_localized(const char* section, const char* key, const char* val) {
	STORE_RESOURCE(section, key, val, set_localized);
}

void Resource::set(const char* section, const char* key, bool val) {
	STORE_RESOURCE(section, key, val, set);
}

void Resource::set(const char* section, const char* key, int val) {
	STORE_RESOURCE(section, key, val, set);
}

void Resource::set(const char* section, const char* key, long val) {
	STORE_RESOURCE(section, key, val, set);
}

void Resource::set(const char* section, const char* key, float val) {
	STORE_RESOURCE(section, key, val, set);
}

void Resource::set(const char* section, const char* key, double val) {
	STORE_RESOURCE(section, key, val, set);
}

EDELIB_NS_END
