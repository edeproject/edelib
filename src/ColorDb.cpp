/*
 * $Id: String.cpp 2953 2009-11-20 15:42:16Z karijes $
 *
 * X11 color database reader
 * Copyright (c) 2005-2011 edelib authors
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

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <edelib/ColorDb.h>
#include <edelib/Debug.h>
#include <edelib/String.h>
#include <edelib/List.h>

EDELIB_NS_BEGIN

/* some default locations */
static const char *x11_rgb[] = {
	"/etc/X11/rgb.txt",
	"/usr/share/X11/rgb.txt",
	"/usr/local/share/X11/rgb.txt",
	"/usr/pkg/X11R6/lib/X11/rgb.txt",
	EDELIB_INSTALL_PREFIX "/share/edelib/colors/rgb.txt",
	0
};

struct ColorInfo {
	unsigned char r, g, b;
	String name;
};

typedef list<ColorInfo> ColorInfoList;
typedef list<ColorInfo>::iterator ColorInfoListIt;

struct ColorDb_P {
	ColorInfoList list;
};

ColorDb::ColorDb() : priv(NULL) {
}

ColorDb::~ColorDb() {
	delete priv;
}

bool ColorDb::load(void) {
	for(int i = 0; x11_rgb[i]; i++)
		if(load(x11_rgb[i]))
			return true;
	return false;
}

bool ColorDb::load(const char *file) {
	E_RETURN_VAL_IF_FAIL(file != NULL, false);

	FILE *fd = fopen(file, "r");
	if(!fd) return false;

	char buf[256], name[64];
	int r, g, b, ret;

	if(!priv) {
		priv = new ColorDb_P;
	} else { 
		/* clear list in case something is there */
		priv->list.clear();
	}

	while(fgets(buf, sizeof(buf), fd) != NULL) {
		ret = sscanf(buf, "%i %i %i\t%63[0-9A-Za-z ]s", &r, &g, &b, name);
		errno = 0;
		if(ret != 4 || errno != 0) {
			E_WARNING(E_STRLOC ": Skipping line as errno was set: '%s' (%i)\n", strerror(errno), errno);
			continue;
		}

		ColorInfo c;
		c.name = name;
		c.r = (unsigned char)r;
		c.g = (unsigned char)g;
		c.b = (unsigned char)b;

		priv->list.push_back(c);
	}

	fclose(fd);
	return true;
}

bool ColorDb::find(const char *name, unsigned char &r, unsigned char &g, unsigned char &b) {
	E_RETURN_VAL_IF_FAIL(priv != NULL, false);
	E_RETURN_VAL_IF_FAIL(priv->list.empty() != true, false);

	ColorInfoListIt it = priv->list.begin(), ite = priv->list.end();
	for(; it != ite; ++it) {
		if((*it).name == name) {
			r = (*it).r;
			g = (*it).g;
			b = (*it).b;

			return true;
		}
	}

	return false;
}

EDELIB_NS_END
