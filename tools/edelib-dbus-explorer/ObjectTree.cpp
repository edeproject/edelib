/*
 * $Id: String.h 2839 2009-09-28 11:36:20Z karijes $
 *
 * Copyright (c) 2005-2012 edelib authors
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

#include <edelib/Debug.h>
#include <edelib/EdbusMessage.h>
#include <edelib/TiXml.h>
#include <string.h>

#include "ObjectTree.h"
#include "icons.h"

EDELIB_NS_USING(EdbusConnection)
EDELIB_NS_USING(EdbusMessage)

#define INTROSPECTABLE_INTERFACE "org.freedesktop.DBus.Introspectable"
#define INTROSPECTABLE_METHOD    "Introspect"

#define STR_CMP(s1, s2) (strcmp((s1), (s2)) == 0)

static void scan_object(EdbusConnection *conn, EdbusMessage &msg, const char *service, const char *path, ObjectTree *self) {
	EdbusMessage reply;
	msg.create_method_call(service, path, INTROSPECTABLE_INTERFACE, INTROSPECTABLE_METHOD);

	if(!conn->send_with_reply_and_block(msg, 1000, reply)) {
		E_WARNING(E_STRLOC ": Did not get reply from service bus. Skipping introspection of '%s' service (object path: '%s')\n", service, path);
		return;
	}

	/* reply must be single element and string, which is xml */
	if(reply.size() != 1) {
		E_WARNING(E_STRLOC ": Expected only one element, but got '%i'\n", reply.size());
		return;
	}

	EdbusMessage::const_iterator it = reply.begin();
	if(!it->is_string()) {
		E_WARNING(E_STRLOC ": Expected string in reply, but got some junk\n");
		return;
	}

	TiXmlDocument doc;
	char buf[128];
	Fl_Tree_Item *titem;

	doc.Parse(it->to_string());
	TiXmlNode *el = doc.FirstChild("node");

	for(el = el->FirstChildElement(); el; el = el->NextSibling()) {
		/* we have subobjects */
		if(STR_CMP(el->Value(), "node")) {
			const char *name = el->ToElement()->Attribute("name");
			if(!name) {
				E_DEBUG(E_STRLOC ": <node> is expected to have 'name' attribute\n");
				continue;
			}

			if(strcmp(path, "/") == 0)
				snprintf(buf, sizeof(buf), "/%s", name);
			else
				snprintf(buf, sizeof(buf), "%s/%s", path, name);

			titem = self->add(buf);
			self->close(titem);
			titem->usericon(&image_package);

			/* recurse */
			scan_object(conn, msg, service, buf, self);
		} else if(STR_CMP(el->Value(), "interface")) {
			/* full interface: get methods and properties */
			const char *name = el->ToElement()->Attribute("name");
			if(!name) {
				E_DEBUG(E_STRLOC ": <interface> is expected to have 'name' attribute\n");
				continue;
			}

			/* append interface to tree */
			snprintf(buf, sizeof(buf), "%s/%s", path, name);
			titem = self->add(buf);
			self->close(titem);
			titem->usericon(&image_interface);

			/* append methods, signals and properties */
			TiXmlNode *sel;
			char buf2[256];

			for(sel = el->FirstChildElement(); sel; sel = sel->NextSibling()) {
				if(STR_CMP(sel->Value(), "method")) {
					name = sel->ToElement()->Attribute("name");
					snprintf(buf2, sizeof(buf2), "%s/%s", buf, name);
					titem = self->add(buf2);
					titem->usericon(&image_method);
					self->close(titem);
				} else if(STR_CMP(sel->Value(), "signal")) {
					name = sel->ToElement()->Attribute("name");
					snprintf(buf2, sizeof(buf2), "%s/%s", buf, name);
					titem = self->add(buf2);
					titem->usericon(&image_signal);
					self->close(titem);
				} else if(STR_CMP(sel->Value(), "property")) {
					name = sel->ToElement()->Attribute("name");
					snprintf(buf2, sizeof(buf2), "%s/%s", buf, name);
					titem = self->add(buf2);
					titem->usericon(&image_property);
					self->close(titem);
				} else {
					E_WARNING(E_STRLOC ": Got unknown node '%s'. Skipping...\n", sel->Value());
				}
			}
		}
	}
}

ObjectTree::ObjectTree(int X, int Y, int W, int H, const char *l) : Fl_Tree(X, Y, W, H, l) {
	showroot(0);
	item_labelbgcolor(color());
}

void ObjectTree::introspect(const char *service, EdbusConnection *c) {
	clear();

	/* first we start with '/', as root object, since all services implement this one */
	EdbusMessage  m;
	scan_object(c, m, service, "/", this);
	redraw();
}
