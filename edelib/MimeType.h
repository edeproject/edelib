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

#ifndef __MIMETYPE_H__
#define __MIMETYPE_H__

#include "String.h"

EDELIB_NS_BEGIN

/**
 * \class MimeType
 * \brief Mime handling class
 *
 * MimeType will handle file types according to specification at http://www.freedesktop.org. This class 
 * depends on <em>shared-mime-info</em> package, usually shipped with todays distributions.
 *
 * It will try to recognize given file looking into $XDG_DATA_DIRS/mime/magic
 * database, consulting $XDG_DATA_DIRS/mime/aliases for potential type * aliasing.
 *
 * Returned values from type() will be in the form <em>context/file-type</em>, so, for example, for given 
 * file foo.tar.gz will be <em>application/x-compressed-tar</em> or for baz.jpg will be <em>image/jpeg</em>.
 *
 * If file could not be recognized, returned string will be <em>application/octet-stream</em>.
 *
 * comment() will return full description for recognized file, looking inside 
 * $XDG_DATA_DIRS/mime/context/file-type.xml file. This description can be plain english (default) or 
 * localized (not implemented yet).
 *
 * \todo add locale during read of comments
 * \todo  Last change in xdgmime.c moved stat-ed code to be executed first so ambiguous directory names
 * (like ".kde" or ".emacs.d" or ".e") doesn't be recognized as files or unknown types; can stat's be delayed?
 */
class EDELIB_API MimeType {
private:
	unsigned char status;
	String mtype;
	String mcmt;
	String micon;

	MimeType(const MimeType&);
	MimeType& operator=(const MimeType&);

public:
	/** Empty constructor */
	MimeType();
	/** Cleans internal data */
	~MimeType();

	/**
	 * Set path to the file for inspection. If file does not exists or is unreadable, it will return false.
	 *
	 * \return true if manage to access file, otherwise false
	 * \param filename is full path to file
	 */
	bool set(const char* filename);

	/** 
	 * Return context/file-type form for file, given with set(). If set() failed, it will return empty string.
	 */
	const String& type(void) const;

	/**
	 * Lookup for description described at the beggining of this document.
	 * If file with description does not exists, it will return empty string.
	 */
	const String& comment(void);

	/**
	 * Returns true if current mime type is subclass of given parameter name.
	 */
	bool subclass_of(const char* mime);

	/**
	 * Try to guess the name of possible icon (the name can be used for edelib::IconLoader and edelib::IconTheme).
	 * Generally the icon used for a mimetype is created based on the mime-type by mapping "/" characters to "-", but 
	 * it can overriden by writing entry in <em>icons</em> file. This file will be searched in user_data_dir()
	 * and system_data_dirs() directories.
	 *
	 * icons file looks like:
	 * \code
	 *  # this is comment
	 *  text/plain:icon-name
	 *  application/octet-stream:another-icon
	 *
	 *  # this is bad and will not be read
	 *  application/octet-stream : icon
	 * \endcode
	 *
	 * \note This rule does not applies for <em>node/xxx</em> (where xxx is folder,socket,fifo, etc.).
	 *       According to the icon naming specs, icon names should be file,socket,folder,...
	 *
	 * \note For folders icon name will be <em>folder</em> due compatibility with icon naming standard.
	 *
	 * If set() failed, it will return empty string.
	 */
	const String& icon_name(void);
}; 

EDELIB_NS_END
#endif // __MIMETYPE_H__
