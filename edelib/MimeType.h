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
 * MimeType will handle file types according to specification
 * at http://www.freedesktop.org. This class depends on
 * <em>shared-mime-info</em> package, usually shipped with todays
 * distributions.
 *
 * It will try to recognize given file looking into $XDG_DATA_DIRS/mime/magic
 * database, consulting $XDG_DATA_DIRS/mime/aliases for potential type
 * aliasing.
 *
 * Returned values from type() will be in form context/file-type, so for
 * example, for given file foo.tar.gz will be <em>application/x-compressed-tar</em>
 * or for baz.jpg will be <em>image/jpeg</em>.
 *
 * If file could not be recognized, returned string will be: <em>application/octet-stream</em>.
 *
 * comment() will return full description for recognized file, looking inside
 * $XDG_DATA_DIRS/mime/context/file-type.xml file. This description can be
 * plain english (default) or localized (not implemented yet).
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
	 * Set path to the file for inspection. If file does not
	 * exists or is unreadable, it will return false.
	 *
	 * \return true if manage to access file, otherwise false
	 * \param filename is full path to file
	 */
	bool set(const char* filename);

	/** 
	 * Return context/file-type form for file, given with set().
	 * If set() failed, it will return empty string.
	 */
	const String& type(void) const;

	/**
	 * Lookup for description described at the beggining of this document.
	 * If file with description does not exists, it will return empty string.
	 */
	const String& comment(void);

	/**
	 * Returns true if current mime type is subclass of given parameter name,
	 * or false if not.
	 */
	bool subclass_of(const char* mime);

	/**
	 * Return the name of potential icon, using name according to freedesktop.org
	 * icon name specification. It will <b>not</b> return file with extension
	 * since further those could be explicitely choosed, <b>nor</b> will return
	 * full path for icons or check if file exists.
	 *
	 * Icon name will be in form: if mime type is image/jpeg, icon will be image-jpeg;
	 * the same will be for eg. application/x-tek-pk, icon name will be application-x-tek-pk.
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
