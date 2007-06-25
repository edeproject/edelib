/*
 * $Id$
 *
 * .desktop file reader and writer
 * Part of edelib.
 * Copyright (c) 2005-2007 EDE Authors.
 *
 * This program is licensed under terms of the 
 * GNU General Public License version 2 or newer.
 * See COPYING for details.
 */

#include <edelib/DesktopFile.h>
#include <edelib/File.h>
#include <edelib/Debug.h>

#include <string.h> // strncmp, strncpy

#define BUFF_SIZE 256
#define DENTRY_KEY  "Desktop Entry"

EDELIB_NAMESPACE {

DesktopFile::DesktopFile() : errcode(DESK_FILE_EMPTY), dtype(DESK_FILE_TYPE_UNKNOWN) {
}

DesktopFile::~DesktopFile() {
}

bool DesktopFile::load(const char* fname) {
	EASSERT(fname != NULL);

	if(!Config::load(fname)) {
		errcode = DESK_FILE_ERR_BAD;
		return false;
	}

	// check Type key, if present, this is .desktop file, otherwise not
	char buff[BUFF_SIZE];
	if(!Config::get(DENTRY_KEY, "Type", buff, BUFF_SIZE)) {
		errcode = DESK_FILE_ERR_BAD;
		return false;
	}

	if(strncmp(buff, "Application", 11) == 0)
		dtype = DESK_FILE_TYPE_APPLICATION;
	else if(strncmp(buff, "Link", 4) == 0)
		dtype = DESK_FILE_TYPE_LINK;
	else if(strncmp(buff, "Directory", 9) == 0)
		dtype = DESK_FILE_TYPE_DIRECTORY;
	else {
		// specs said that if none of above values are find, file is not .desktop file
		dtype = DESK_FILE_TYPE_UNKNOWN;
		errcode = DESK_FILE_ERR_BAD;
		return false;
	}

	errcode = DESK_FILE_SUCCESS;
	return true;
}

bool DesktopFile::save(const char* fname) {
	EASSERT(fname != NULL);

	if(errcode != DESK_FILE_SUCCESS && errcode != DESK_FILE_EMPTY)
		return false;
	return Config::save(fname);
}

DesktopFileType DesktopFile::type(void) {
	if(errcode != DESK_FILE_SUCCESS)
		return DESK_FILE_TYPE_UNKNOWN;
	return dtype;
}

bool DesktopFile::name(char* val, int len) {
	if(errcode != DESK_FILE_SUCCESS)
		return false;
	// TODO: Name key is required too; place it next to Type
	if(!Config::get_localized(DENTRY_KEY, "Name", val, len))
		return false;
	return true;
}

bool DesktopFile::generic_name(char* val, int len) {
	if(errcode != DESK_FILE_SUCCESS)
		return false;
	if(!Config::get_localized(DENTRY_KEY, "GenericName", val, len))
		return false;
	return true;
}

bool DesktopFile::comment(char* val, int len) {
	if(errcode != DESK_FILE_SUCCESS)
		return false;
	if(!Config::get_localized(DENTRY_KEY, "Comment", val, len))
		return false;
	return true;
}

bool DesktopFile::icon(char* val, int len) {
	if(errcode != DESK_FILE_SUCCESS)
		return false;
	if(!Config::get_localized(DENTRY_KEY, "Icon", val, len))
		return false;
	return true;
}

bool DesktopFile::exec(char* val, int len) {
	if(errcode != DESK_FILE_SUCCESS)
		return false;
	if(!Config::get(DENTRY_KEY, "Exec", val, len))
		return false;
	return true;
}

bool DesktopFile::try_exec(char* val, int len) {
	if(errcode != DESK_FILE_SUCCESS)
		return false;

	char* buff = new char[256];

	if(!Config::get(DENTRY_KEY, "TryExec", buff, 256)) {
		delete [] buff;
		return false;
	}

	String path = file_path(buff);
	if(path.empty()) {
		delete [] buff;
		return false;
	}

	strncpy(val, buff, len);
	delete [] buff;
	return true;
}

bool DesktopFile::path(char* val, int len) {
	if(errcode != DESK_FILE_SUCCESS)
		return false;
	if(dtype != DESK_FILE_TYPE_APPLICATION)
		return false;

	if(!Config::get(DENTRY_KEY, "Path", val, len))
		return false;
	return true;
}

bool DesktopFile::url(char* val, int len) {
	if(errcode != DESK_FILE_SUCCESS)
		return false;
	if(dtype != DESK_FILE_TYPE_LINK)
		return false;

	if(!Config::get(DENTRY_KEY, "URL", val, len))
		return false;
	return true;
}

bool DesktopFile::mime_type(char* val, int len) {
	if(errcode != DESK_FILE_SUCCESS)
		return false;
	if(!Config::get(DENTRY_KEY, "MimeType", val, len))
		return false;
	return true;
}

bool DesktopFile::no_display(void) {
	if(errcode != DESK_FILE_SUCCESS)
		return false;

	bool ret;
	Config::get(DENTRY_KEY, "NoDisplay", ret, false);
	return ret;
}

bool DesktopFile::hidden(void) {
	if(errcode != DESK_FILE_SUCCESS)
		return false;

	bool ret;
	Config::get(DENTRY_KEY, "Hidden", ret, false);
	return ret;
}

bool DesktopFile::terminal(void) {
	if(errcode != DESK_FILE_SUCCESS)
		return false;

	bool ret;
	Config::get(DENTRY_KEY, "Terminal", ret, false);
	return ret;
}

bool DesktopFile::startup_notify(void) {
	if(errcode != DESK_FILE_SUCCESS)
		return false;

	bool ret;
	Config::get(DENTRY_KEY, "StartupNotify", ret, false);
	return ret;
}

void DesktopFile::set_type(DesktopFileType t) {
	if(errcode != DESK_FILE_SUCCESS && errcode != DESK_FILE_EMPTY)
		return;

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
			// what to do ???
			EASSERT(0 && "Feeding me with bad DesktopFileType value! Now you got it >:D");
			break;
	}

	Config::set(DENTRY_KEY, "Type", val);
}

#define SET_LOCALIZED(key, val) \
	if(errcode != DESK_FILE_SUCCESS && errcode != DESK_FILE_EMPTY) \
		return; \
	Config::set_localized(DENTRY_KEY, key, val)

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
	if(errcode != DESK_FILE_SUCCESS && errcode != DESK_FILE_EMPTY) \
		return; \
	Config::set(DENTRY_KEY, key, val)

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

}
