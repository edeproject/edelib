/*
 * $Id$
 *
 * Resource reader and writer
 * Copyright (c) 2008-2009 edelib authors
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

#include <unistd.h>
#include <edelib/Resource.h>
#include <edelib/Config.h>
#include <edelib/Debug.h>
#include <edelib/Util.h>
#include <edelib/File.h>
#include <edelib/Directory.h>

#define CONFIG_FILE_SUFFIX ".conf"

EDELIB_NS_BEGIN

typedef list<String> StrList;
typedef list<String>::iterator StrListIter;

static bool locate_resource_sys(const char* name, String& path, bool is_config) {
	StrList dirs;

	if(is_config)
		system_config_dirs(dirs);
	else
		system_data_dirs(dirs);

	StrListIter it = dirs.begin(), it_end = dirs.end();
	
	for(; it != it_end; ++it) {
		*it += E_DIR_SEPARATOR;
		*it += name;

		if(access((*it).c_str(), F_OK) == 0) {
			path = *it;
			return true;
		}
	}

	path.clear();
	return false; 
}

static bool locate_resource_user(const char* name, String& path, bool is_config) {
	if(is_config)
		path = user_config_dir();
	else
		path = user_data_dir();

	path += E_DIR_SEPARATOR;
	path += name;

	if(access(path.c_str(), F_OK) == 0)
		return true;

	path.clear();
	return false;
}

static String locate_resource(const char* name, ResourceType r, bool is_config) {
	String ret;

	switch(r) {
		case RES_USER_ONLY:
			/* can be empty; intentional */
			locate_resource_user(name, ret, is_config);
			break;
		case RES_SYS_ONLY:
			/* can be empty; intentional */
			locate_resource_sys(name, ret, is_config);
			break;
		case RES_USER_FIRST:
			if(!locate_resource_user(name, ret, is_config))
				locate_resource_sys(name, ret, is_config);
			break;
		case RES_SYS_FIRST:
			if(!locate_resource_sys(name, ret, is_config))
				locate_resource_user(name, ret, is_config);
			break;
		default:
			E_ASSERT(0 && "Unknown resource type");
	}

	return ret;
}

static void construct_name_from_domain(const char* domain, const char* prefix, String& ret) {
	ret = prefix;
	ret += E_DIR_SEPARATOR;
	ret += domain;
	ret += CONFIG_FILE_SUFFIX;
}

Resource::Resource() : sys_conf(NULL), user_conf(NULL) {
}

Resource::~Resource() {
	clear();
}

bool Resource::load(const char* domain, const char* prefix) {
	E_ASSERT(domain != NULL);
	E_ASSERT(prefix != NULL);

	clear();

	String path, file;
	construct_name_from_domain(domain, prefix, file);

	/* check first in system directories */
	if(locate_resource_sys(file.c_str(), path, true)) {
		sys_conf = new Config;

		if(!sys_conf->load(path.c_str())) {
			delete sys_conf;
			sys_conf = NULL;
		}
	}

	path.clear();

	/* now go to user directory */
	if(locate_resource_user(file.c_str(), path, true)) {
		user_conf = new Config;

		if(!user_conf->load(path.c_str())) {
			delete user_conf;
			user_conf = NULL;
		}
	}

	return (sys_conf != NULL || user_conf != NULL);
}

bool Resource::save(const char* domain, const char* prefix) {
	E_ASSERT(domain != NULL);
	E_ASSERT(prefix != NULL);
	E_RETURN_VAL_IF_FAIL(user_conf != NULL, false);

	String path, file;
	construct_name_from_domain(domain, prefix, file);

	path = user_config_dir();
	path += E_DIR_SEPARATOR;
	path += file;

	/* Create directory if necessary */
	String::size_type loc = 0, loc2 = 0;

	/* find last slash */
	while((loc2 = path.find('/', loc + 1)) != String::npos) 
		loc = loc2;

	String tmp = path.substr(0, loc);
	const char* dname = tmp.c_str();

	if(!dir_create_with_parents(dname, 0700))
		return false;

	return user_conf->save(path.c_str());
}

void Resource::clear(void) {
	delete sys_conf;
	sys_conf = NULL;

	delete user_conf;
	user_conf = NULL;
}

#define FETCH_STR_RESOURCE(section, key, ret, size, rt, get_func)         \
do {                                                                      \
	switch(rt) {                                                          \
		case RES_SYS_ONLY:                                                \
			if(sys_conf)                                                  \
				return sys_conf->get_func(section, key, ret, size);       \
			return false;                                                 \
		case RES_USER_ONLY:                                               \
			if(user_conf)                                                 \
				return user_conf->get_func(section, key, ret, size);      \
			return false;                                                 \
		case RES_SYS_FIRST:                                               \
			if(sys_conf && sys_conf->get_func(section, key, ret, size))   \
				return true;                                              \
			if(user_conf && user_conf->get_func(section, key, ret, size)) \
				return true;                                              \
			return false;                                                 \
		case RES_USER_FIRST:                                              \
			if(user_conf && user_conf->get_func(section, key, ret, size)) \
				return true;                                              \
			if(sys_conf && sys_conf->get_func(section, key, ret, size))   \
				return true;                                              \
			return false;                                                 \
		default:                                                          \
			E_ASSERT(0 && "Unknown resource type");                        \
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
	switch(rt) {                                                    \
		case RES_SYS_ONLY:                                          \
			if(sys_conf)                                            \
				return sys_conf->get(section, key, ret, dfl);       \
			ret = dfl;                                              \
			return false;                                           \
		case RES_USER_ONLY:                                         \
			if(user_conf)                                           \
				return user_conf->get(section, key, ret, dfl);      \
			ret = dfl;                                              \
			return false;                                           \
		case RES_SYS_FIRST:                                         \
			if(sys_conf && sys_conf->get(section, key, ret, dfl))   \
				return true;                                        \
			if(user_conf && user_conf->get(section, key, ret, dfl)) \
				return true;                                        \
			ret = dfl;                                              \
			return false;                                           \
		case RES_USER_FIRST:                                        \
			if(user_conf && user_conf->get(section, key, ret, dfl)) \
				return true;                                        \
			if(sys_conf && sys_conf->get(section, key, ret, dfl))   \
				return true;                                        \
			ret = dfl;                                              \
			return false;                                           \
		default:                                                    \
			E_ASSERT(0 && "Unknown resource type");                  \
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
		
String Resource::find_config(const char* domain, ResourceType r, const char* prefix) {
	E_ASSERT(domain != NULL);
	E_ASSERT(prefix != NULL);

	String n;
	construct_name_from_domain(domain, prefix, n);

	String ret = locate_resource(n.c_str(), r, true);
	E_RETURN_VAL_IF_FAIL(file_exists(ret.c_str()), "");
	return ret;
}

String Resource::find_data(const char* name, ResourceType r) {
	E_ASSERT(name != NULL);
	String ret = locate_resource(name, r, false);
	return ret;
}

EDELIB_NS_END
