/*
 * $Id$
 *
 * Mime handling class.
 * Part of edelib.
 * Copyright (c) 2005-2007 EDE Authors.
 *
 * This program is licenced under terms of the 
 * GNU General Public Licence version 2 or newer.
 * See COPYING for details.
 */

#include <edelib/MimeType.h>
#include <edelib/TiXml.h>
#include <edelib/Debug.h>
#include <edelib/Util.h>
#include <edelib/StrUtil.h>
#include <edelib/List.h>
#include "xdgmime/xdgmime.h"

#include <string.h>  // strncmp

//#define MIME_DIR "/usr/share/mime"

#define MIME_LOADED    1
#define COMMENT_LOADED 2
#define ICON_LOADED    4

EDELIB_NS_BEGIN

MimeType::MimeType() : status(0)
{
}

MimeType::~MimeType()
{
	xdg_mime_shutdown();
}

bool MimeType::set(const char* filename) {
	EASSERT(filename != NULL);
	/*
	 * Call on the same file again check.
	 *
	 * FIXME: some stamp check would be nice here so if file 
	 * with the same name change content, can be inspected.
	 */
	if(mtype == filename)
		return true;

	mcmt.clear(); mtype.clear(); micon.clear();

	const char* res = xdg_mime_get_mime_type_for_file(filename, NULL);

	if(!res) {
		status = 0;
		return false;
	}

	mtype.assign(res);
	status = MIME_LOADED;
	return true;
}

const String& MimeType::type(void) const {
	// we already cleared value
	return mtype;
}

const String& MimeType::comment(void) {
	// calling without mime loaded do nothing
	if(!(status & MIME_LOADED))
		return mcmt;

	if(status & COMMENT_LOADED)
		return mcmt;

	String ttype = mtype;
	ttype += ".xml";

#if 0
	String path = build_filename("/", MIME_DIR, ttype.c_str());
	if(!file_exists(path.c_str())) {
		EDEBUG(ESTRLOC ": MimeType::comment() %s does not exists\n", path.c_str());
		return mcmt;
	}
#endif

	const char* p = xdg_mime_find_data(ttype.c_str());
	if(!p)
		return mcmt;

	String path = p;
	
	TiXmlDocument doc(path.c_str());

	if(!doc.LoadFile()) {
		EDEBUG(ESTRLOC ": MimeType::comment() %s malformed\n", path.c_str());
		return mcmt;
	}

	TiXmlNode* el = doc.FirstChild("mime-type");

	/*
	 * First element which does not have "xml:lang" attribute
	 * is default one. So hunt that one :)
	 *
	 * Btw. TinyXML does not handle XML namespaces and will return
	 * "<namespace>:<attribute>" value. This is not big deal as long
	 * as correctly fetch attribute values.
	 */
	for(el = el->FirstChildElement(); el; el = el->NextSibling()) {
		if(strncmp(el->Value(), "comment", 7) == 0) {
			// TODO: add locale here
			if(!el->ToElement()->Attribute("xml:lang")) {
				TiXmlText* data = el->FirstChild()->ToText();
				if(data) {
					mcmt = data->Value();
					break;
				}
			}
		}
	}

	status |= COMMENT_LOADED;
	return mcmt;
}

const String& MimeType::icon_name(void) {
	if(!(status & MIME_LOADED) && mtype.empty())
		return micon;

	if(status & ICON_LOADED)
		return micon;

	list<String> vs;
	stringtok(vs, mtype, "/");

	// failed, accepted is only type/name
	if(vs.size() != 2)
		return micon;

	micon.clear();
	micon.reserve(5);
	list<String>::iterator it = vs.begin();

	if((*it) == "inode") {
		/*
		 * XDG Mime guys decided that folders are named under 'inode/directory.xml'. 
		 * But icon naming standard say it should contain name 'folder' (sigh!).
		 */
		++it;
		if((*it) == "directory")
			micon = "folder";
		else
			micon = (*it);
	} else {
		micon = (*it);
		micon += '-';
		++it;
		micon += (*it);
	}

	status |= ICON_LOADED;
	return micon;
}

EDELIB_NS_END
