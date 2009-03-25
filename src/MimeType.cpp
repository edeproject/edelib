/*
 * $Id$
 *
 * Mime handling class
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

#include <string.h>  // strncmp

#include <edelib/MimeType.h>
#include <edelib/TiXml.h>
#include <edelib/Debug.h>
#include <edelib/Util.h>
#include <edelib/StrUtil.h>
#include <edelib/List.h>

#include "xdgmime/xdgmime.h"

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
	E_ASSERT(filename != NULL);
	/*
	 * Call on the same file again check.
	 * FIXME: some stamp check would be nice here so if file  with the same name change content, can be inspected.
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

	const char* p = xdg_mime_find_data(ttype.c_str());
	if(!p)
		return mcmt;

	String path = p;
	
	TiXmlDocument doc(path.c_str());

	if(!doc.LoadFile()) {
		E_DEBUG(E_STRLOC ": MimeType::comment() %s malformed\n", path.c_str());
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

bool MimeType::subclass_of(const char* mime) {
	E_ASSERT(mime != NULL);

	if(!(status & MIME_LOADED))
		return false;

	return (xdg_mime_mime_type_subclass(mtype.c_str(), mime) == 1);
}

const String& MimeType::icon_name(void) {
	if(!(status & MIME_LOADED) && mtype.empty())
		return micon;

	if(status & ICON_LOADED)
		return micon;

	/* check first for user aliases */
	const char* ic = xdg_mime_get_icon(mtype.c_str());
	if(ic) {
		micon = ic;

		status |= ICON_LOADED;
		return micon;
	}

	/* clear whatever we had previous */
	micon.clear();

	String::size_type pos = mtype.find('/', 0);
	if(pos == String::npos)
		return micon;

	/*
	 * XDG Mime guys decided that folders are named under 'inode/directory.xml'. 
	 * But icon naming standard say it should contain name 'folder' (sigh!).
	 *
	 * FIXME: strncmp() here?
	 */
	if(mtype.substr(0, pos) == "inode") {
		micon = mtype.substr(pos + 1);
		if(micon == "directory")
			micon = "folder";
	} else {
		micon = mtype;
		micon[pos] = '-';
	}

	status |= ICON_LOADED;
	return micon;
}

EDELIB_NS_END
