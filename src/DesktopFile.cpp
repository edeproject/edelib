/*
 * $Id$
 *
 * .desktop file reader and writer
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

#include <string.h> // strncmp, strncpy

#include <edelib/DesktopFile.h>
#include <edelib/File.h>
#include <edelib/Debug.h>
#include <edelib/StrUtil.h>

#define BUF_SIZE   256
#define ENTRY_SECT  "Desktop Entry"

EDELIB_NS_BEGIN

DesktopFile::DesktopFile() : errcode(DESK_FILE_EMPTY), dtype(DESK_FILE_TYPE_UNKNOWN) {
}

DesktopFile::~DesktopFile() {
}

bool DesktopFile::load(const char* fname) {
	E_ASSERT(fname != NULL);

	if(!Config::load(fname)) {
		errcode = DESK_FILE_ERR_BAD;
		return false;
	}

	char buf[BUF_SIZE];

	/* check if Type key, if present, this is .desktop file, otherwise not */
	if(!Config::get(ENTRY_SECT, "Type", buf, BUF_SIZE)) {
		errcode = DESK_FILE_ERR_BAD;
		return false;
	}

	if(strncmp(buf, "Application", 11) == 0)
		dtype = DESK_FILE_TYPE_APPLICATION;
	else if(strncmp(buf, "Link", 4) == 0)
		dtype = DESK_FILE_TYPE_LINK;
	else if(strncmp(buf, "Directory", 9) == 0)
		dtype = DESK_FILE_TYPE_DIRECTORY;
	else {
		/* specs said that if none of above values are find, file is not .desktop file */
		dtype = DESK_FILE_TYPE_UNKNOWN;
		errcode = DESK_FILE_ERR_BAD;
		return false;
	}

	errcode = DESK_FILE_SUCCESS;
	return true;
}

bool DesktopFile::save(const char* fname) {
	E_RETURN_VAL_IF_FAIL(errcode == DESK_FILE_SUCCESS /* || errcode == DESK_FILE_EMPTY */, false);
	return Config::save(fname);
}

void DesktopFile::create_new(DesktopFileType t) {
	clear();
	dtype = t;
	const char* val = NULL;

	switch(dtype) {
		case DESK_FILE_TYPE_APPLICATION:
			val = "Application";
			break;
		case DESK_FILE_TYPE_LINK:
			val = "Link";
			break;
		case DESK_FILE_TYPE_DIRECTORY:
			val = "Directory";
		default:
			E_ASSERT(0 && "Feeding me with invalid type value! Make sure type is application, link or directory");
	}

	Config::set(ENTRY_SECT, "Type", val);

	/* now mark it as valid */
	errcode = DESK_FILE_SUCCESS;
}

DesktopFileType DesktopFile::type(void) {
	E_RETURN_VAL_IF_FAIL(errcode == DESK_FILE_SUCCESS, DESK_FILE_TYPE_UNKNOWN);
	return dtype;
}

bool DesktopFile::name(char* val, int len) {
	E_RETURN_VAL_IF_FAIL(errcode == DESK_FILE_SUCCESS, false);

	/* TODO: Name key is required too; place it next to Type */
	if(!Config::get_localized(ENTRY_SECT, "Name", val, len))
		return false;
	return true;
}

bool DesktopFile::generic_name(char* val, int len) {
	E_RETURN_VAL_IF_FAIL(errcode == DESK_FILE_SUCCESS, false);

	if(!Config::get_localized(ENTRY_SECT, "GenericName", val, len))
		return false;
	return true;
}

bool DesktopFile::comment(char* val, int len) {
	E_RETURN_VAL_IF_FAIL(errcode == DESK_FILE_SUCCESS, false);

	if(!Config::get_localized(ENTRY_SECT, "Comment", val, len))
		return false;
	return true;
}

bool DesktopFile::icon(char* val, int len) {
	E_RETURN_VAL_IF_FAIL(errcode == DESK_FILE_SUCCESS, false);

	if(!Config::get_localized(ENTRY_SECT, "Icon", val, len))
		return false;
	return true;
}

bool DesktopFile::exec(char* val, int len) {
	E_RETURN_VAL_IF_FAIL(errcode == DESK_FILE_SUCCESS, false);

	char buf[BUF_SIZE];

	if(!Config::get(ENTRY_SECT, "Exec", buf, BUF_SIZE))
		return false;

	char*  pos;
	String exec_path;

	/* remove possible parameters so the path can be deduced correctly */
	if((pos = strchr(buf, ' ')) || (pos = strchr(buf, '\t'))) {
		String tmp;

		tmp.assign(buf, pos - buf);
		exec_path = file_path(tmp.c_str());

		/* the spec requries executable must not have '=' */
		if(exec_path.empty() || (exec_path.find('=', 0) != String::npos))
			return false;

		/* append remaining parameters */
		exec_path += pos;
	} else {
		exec_path = file_path(buf);

		/* the spec requries executable must not have '=' */
		if(exec_path.empty() || (exec_path.find('=', 0) != String::npos))
			return false;
	}

	strncpy(val, exec_path.c_str(), len);
	val[len - 1] = '\0';
	return true;
}

bool DesktopFile::try_exec(bool& program_found) {
	E_RETURN_VAL_IF_FAIL(errcode == DESK_FILE_SUCCESS, false);

	char buf[BUF_SIZE];

	if(!Config::get(ENTRY_SECT, "TryExec", buf, BUF_SIZE))
		return false;

	String exec_path = file_path(buf);
	if(exec_path.empty()) {
		program_found = false;
		return true;
	}

	program_found = true;
	return true;
}

bool DesktopFile::path(char* val, int len) {
	E_RETURN_VAL_IF_FAIL(errcode == DESK_FILE_SUCCESS, false);
	E_RETURN_VAL_IF_FAIL(dtype == DESK_FILE_TYPE_APPLICATION, false);

	if(!Config::get(ENTRY_SECT, "Path", val, len))
		return false;
	return true;
}

bool DesktopFile::url(char* val, int len) {
	E_RETURN_VAL_IF_FAIL(errcode == DESK_FILE_SUCCESS, false);
	E_RETURN_VAL_IF_FAIL(dtype == DESK_FILE_TYPE_LINK, false);

	if(!Config::get(ENTRY_SECT, "URL", val, len))
		return false;
	return true;
}

bool DesktopFile::mime_type(char* val, int len) {
	E_RETURN_VAL_IF_FAIL(errcode == DESK_FILE_SUCCESS, false);

	if(!Config::get(ENTRY_SECT, "MimeType", val, len))
		return false;
	return true;
}

bool DesktopFile::no_display(void) {
	E_RETURN_VAL_IF_FAIL(errcode == DESK_FILE_SUCCESS, false);

	bool ret;
	Config::get(ENTRY_SECT, "NoDisplay", ret, false);
	return ret;
}

bool DesktopFile::hidden(void) {
	E_RETURN_VAL_IF_FAIL(errcode == DESK_FILE_SUCCESS, false);

	bool ret;
	Config::get(ENTRY_SECT, "Hidden", ret, false);
	return ret;
}

bool DesktopFile::terminal(void) {
	E_RETURN_VAL_IF_FAIL(errcode == DESK_FILE_SUCCESS, false);

	bool ret;
	Config::get(ENTRY_SECT, "Terminal", ret, false);
	return ret;
}

bool DesktopFile::startup_notify(void) {
	E_RETURN_VAL_IF_FAIL(errcode == DESK_FILE_SUCCESS, false);

	bool ret;
	Config::get(ENTRY_SECT, "StartupNotify", ret, false);
	return ret;
}

bool DesktopFile::only_show_in(char* val, int len) {
	E_RETURN_VAL_IF_FAIL(errcode == DESK_FILE_SUCCESS, false);

	if(!Config::get(ENTRY_SECT, "OnlyShowIn", val, len))
		return false;
	return true;
}

bool DesktopFile::only_show_in(list<String>& lst) {
	E_RETURN_VAL_IF_FAIL(errcode == DESK_FILE_SUCCESS, false);

	char buff[256];
	if(!Config::get(ENTRY_SECT, "OnlyShowIn", buff, sizeof(buff)-1))
		return false;

	stringtok(lst, buff, ";");
	return true;
}

bool DesktopFile::not_show_in(char* val, int len) {
	E_RETURN_VAL_IF_FAIL(errcode == DESK_FILE_SUCCESS, false);

	if(!Config::get(ENTRY_SECT, "NotShowIn", val, len))
		return false;
	return true;
}

bool DesktopFile::not_show_in(list<String>& lst) {
	E_RETURN_VAL_IF_FAIL(errcode == DESK_FILE_SUCCESS, false);

	char buff[256];
	if(!Config::get(ENTRY_SECT, "NotShowIn", buff, sizeof(buff)-1))
		return false;

	stringtok(lst, buff, ";");
	return true;
}

void DesktopFile::set_type(DesktopFileType t) {
	E_RETURN_IF_FAIL(errcode == DESK_FILE_SUCCESS || errcode == DESK_FILE_EMPTY);

	const char* val = 0;
	switch(t) {
		case DESK_FILE_TYPE_APPLICATION:
			val = "Application";
			break;
		case DESK_FILE_TYPE_LINK:
			val = "Link";
			break;
		case DESK_FILE_TYPE_DIRECTORY:
			val = "Directory";
			break;
		default:
			E_ASSERT(0 && "Feeding me with invalid type value! Make sure type is application, link or directory");
			break;
	}

	Config::set(ENTRY_SECT, "Type", val);
}

#define SET_LOCALIZED(key, val) \
	E_RETURN_IF_FAIL(errcode == DESK_FILE_SUCCESS || errcode == DESK_FILE_EMPTY); \
	Config::set_localized(ENTRY_SECT, key, val)

void DesktopFile::set_name(const char* val) {
	SET_LOCALIZED("Name", val);
}

void DesktopFile::set_generic_name(const char* val) {
	SET_LOCALIZED("GenericName", val);
}

void DesktopFile::set_comment(const char* val) {
	SET_LOCALIZED("Comment", val);
}

void DesktopFile::set_icon(const char* val) {
	SET_LOCALIZED("Icon", val);
}

#define SET_KEY(key, val) \
	E_RETURN_IF_FAIL(errcode == DESK_FILE_SUCCESS || errcode == DESK_FILE_EMPTY); \
	Config::set(ENTRY_SECT, key, val)

void DesktopFile::set_exec(const char* val) {
	SET_KEY("Exec", val);
}

void DesktopFile::set_try_exec(const char* val) {
	SET_KEY("TryExec", val);
}

void DesktopFile::set_path(const char* val) {
	SET_KEY("Path", val);
}

void DesktopFile::set_url(const char* val) {
	SET_KEY("URL", val);
}

void DesktopFile::set_mime_type(const char* val) {
	SET_KEY("MimeType", val);
}

void DesktopFile::set_no_display(bool val) {
	SET_KEY("NoDisplay", val);
}

void DesktopFile::set_hidden(bool val) {
	SET_KEY("Hidden", val);
}

void DesktopFile::set_terminal(bool val) {
	SET_KEY("Terminal", val);
}

void DesktopFile::set_startup_notify(bool val) {
	SET_KEY("StartupNotify", val);
}

void DesktopFile::set_only_show_in(const list<String>& lst) {
	/* 
	 * Desktop entry specs requires only one of OnlyShowIn or NotShowIn
	 * can exists in one section 
	 */
	if(Config::key_exist(ENTRY_SECT, "NotShowIn"))
		return;

	if(lst.empty())
		return;

	if(errcode != DESK_FILE_SUCCESS && errcode != DESK_FILE_EMPTY)
		return;

	String all;
	all.reserve(256);
	list<String>::const_iterator it = lst.begin(), it_end = lst.end();

	for(; it != it_end; ++it) {
		all += (*it);
		all += ';'; // intentionally since value must ends with ':'
	}

	Config::set(ENTRY_SECT, "OnlyShowIn", all.c_str());
}

void DesktopFile::set_not_show_in(const list<String>& lst) {
	/* 
	 * Desktop entry specs requires only one of OnlyShowIn or NotShowIn
	 * can exists in one section 
	 */
	if(Config::key_exist(ENTRY_SECT, "OnlyShowIn"))
		return;

	if(lst.empty())
		return;

	if(errcode != DESK_FILE_SUCCESS && errcode != DESK_FILE_EMPTY)
		return;

	String all;
	all.reserve(256);
	list<String>::const_iterator it = lst.begin(), it_end = lst.end();

	for(; it != it_end; ++it) {
		all += (*it);
		all += ';'; // intentionally since value must ends with ':'
	}

	Config::set(ENTRY_SECT, "NotShowIn", all.c_str());
}

EDELIB_NS_END
