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

#ifndef __MIMETYPE_H__
#define __MIMETYPE_H__

#include "econfig.h"
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
