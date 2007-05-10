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
#include <edelib/File.h>
#include <string.h>  // strncmp
#include "xdgmime/xdgmime.h"

#define MIME_DIR "/usr/share/mime"

EDELIB_NAMESPACE {

MimeType::MimeType()
{
}

MimeType::~MimeType()
{
	xdg_mime_shutdown();
}

bool MimeType::set(const char* filename)
{
	EASSERT(filename != NULL);

	mcmt.clear(); mtype.clear();

	const char* res = xdg_mime_get_mime_type_for_file(filename, NULL);
	if(!res)
		return false;

	mtype.assign(res);

	return true;
}

String MimeType::type(void)
{
	return mtype;
}

String MimeType::comment(void)
{
	String ttype = mtype;
	ttype += ".xml";
	String path = build_filename("/", MIME_DIR, ttype.c_str());

	if(!file_exists(path.c_str())) {
		EDEBUG("MimeType::comment() %s does not exists\n", path.c_str());
		return "";
	}
	
	TiXmlDocument doc(path.c_str());

	if(!doc.LoadFile()) {
		EDEBUG("MimeType::comment() %s malformed\n", path.c_str());
		return "";
	}

	TiXmlNode* el = doc.FirstChild("mime-type");

	//printf("--> %s\n", el->ToElement()->Attribute("type"));

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

	return mcmt;
}

}
