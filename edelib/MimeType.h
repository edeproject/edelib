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

EDELIB_NAMESPACE {

class MimeType
{
	private:
		String mtype;
		String mcmt;

		MimeType(const MimeType&);
		MimeType& operator=(const MimeType&);

	public:
		MimeType();
		~MimeType();

		bool set(const char* filename);
		String type(void);
		String comment(void);
}; 

}
#endif
