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

#include <string.h>
#include <FL/Fl.H>
#include <FL/Fl_Tooltip.H>
#include <edelib/Debug.h>
#include <edelib/EdbusMessage.h>
#include <edelib/TiXml.h>
#include <edelib/List.h>
#include <edelib/MenuItem.h>
#include <edelib/Nls.h>
#include <edelib/String.h>

#include "ObjectTree.h"
#include "ScriptEditor.h"
#include "Entity.h"
#include "icons.h"

EDELIB_NS_USING(EdbusConnection)
EDELIB_NS_USING(EdbusMessage)
EDELIB_NS_USING(MenuItem)
EDELIB_NS_USING(MenuButton)
EDELIB_NS_USING(String)

#define INTROSPECTABLE_INTERFACE "org.freedesktop.DBus.Introspectable"
#define INTROSPECTABLE_METHOD    "Introspect"

#define STR_CMP(s1, s2)     (strcmp((s1), (s2)) == 0)
#define STR_CMP_VALUE(o, s) (strcmp((o)->Value(), (s)) == 0)

static char tooltip_buf[128];
static void send_to_editor_cb(Fl_Widget*, void*);
static void describe_in_editor_cb(Fl_Widget*, void*);

static MenuItem menu_[] = {
	{_("&Send to editor"), 0, send_to_editor_cb, 0},
	{_("&Describe"), 0, describe_in_editor_cb, 0},
	{0}
};

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
		if(STR_CMP_VALUE(el, "node")) {
			const char *name = el->ToElement()->Attribute("name");
			if(!name) {
				E_DEBUG(E_STRLOC ": <node> is expected to have 'name' attribute\n");
				continue;
			}

			/* TODO: maybe use EdbusObjectPath? */
			if(strcmp(path, "/") == 0)
				snprintf(buf, sizeof(buf), "/%s", name);
			else
				snprintf(buf, sizeof(buf), "%s/%s", path, name);

			titem = self->add(buf);
			self->close(titem);
			titem->usericon(&image_package);

			/* recurse */
			scan_object(conn, msg, service, buf, self);
		} else if(STR_CMP_VALUE(el, "interface")) {
			/* full interface: get methods and properties */
			const char *name = el->ToElement()->Attribute("name");
			if(!name) {
				E_DEBUG(E_STRLOC ": <interface> is expected to have 'name' attribute\n");
				continue;
			}

			String interface_str = name;
			String object_path_str = path;

			/* append interface to tree */
			snprintf(buf, sizeof(buf), "%s/%s", path, name);
			titem = self->add(buf);
			self->close(titem);
			titem->usericon(&image_interface);

			/* append methods, signals and properties */
			TiXmlNode *sel;
			char buf2[256];
			Fl_Image *icon;
			EntityType et;

			for(sel = el->FirstChildElement(); sel; sel = sel->NextSibling()) {
				if(STR_CMP_VALUE(sel, "method")) {
					icon = &image_method;
					et = ENTITY_METHOD;
				} else if(STR_CMP_VALUE(sel, "signal")) {
					icon = &image_signal;
					et = ENTITY_SIGNAL;
				} else if(STR_CMP_VALUE(sel, "property")) {
					icon = &image_property;
					et = ENTITY_PROPERTY;
				} else {
					E_WARNING(E_STRLOC ": Got unknown node '%s'. Skipping...\n", sel->Value());
					continue;
				}

				/* everything else are common elements between different types */
				name = sel->ToElement()->Attribute("name");
				snprintf(buf2, sizeof(buf2), "%s/%s", buf, name);
				titem = self->add(buf2);
				titem->usericon(icon);
				self->close(titem);

				/* fill our metadata */
				Entity *en = new Entity();
				en->set_type(et);
				en->set_name(name);
				en->set_interface(interface_str.c_str());
				en->set_path(object_path_str.c_str());

				if(et == ENTITY_PROPERTY) {
					const char *argstype, *argsname, *argsaccess;
					argstype = sel->ToElement()->Attribute("type");
					argsname = sel->ToElement()->Attribute("name");
					argsaccess = sel->ToElement()->Attribute("access");

					en->append_arg(argsname, argstype, DIRECTION_NONE, argsaccess);
				} else {
					TiXmlNode *argsnode;
					for(argsnode = sel->FirstChildElement(); argsnode; argsnode = argsnode->NextSibling()) {
						if(!STR_CMP_VALUE(argsnode, "arg")) {
							E_WARNING(E_STRLOC ": Expecting 'arg' node, but got '%s'. Skipping...\n", argsnode->Value());
							continue;
						}

						const char *argstype, *argsname, *argsdirection;
						ArgDirection dir = DIRECTION_NONE;

						argstype = argsnode->ToElement()->Attribute("type");
						if(!argstype) continue;

						argsname = argsnode->ToElement()->Attribute("name");
						if(!argsname) continue;

						/* it is fine to not have direction, which means it is only a method */
						argsdirection = argsnode->ToElement()->Attribute("direction");
						if(argsdirection) {
							if(STR_CMP(argsdirection, "in"))
								dir = DIRECTION_IN;
							else if(STR_CMP(argsdirection, "out"))
								dir = DIRECTION_OUT;
						}

						en->append_arg(argsname, argstype, dir);
					}
				}

				/* put it inside our tree so we can manage it */
				self->append_entity(en);
				/* add also inside Fl_Tree_Item */
				titem->user_data(en);
			}
		}
	}
}

static void send_to_editor_cb(Fl_Widget*, void *s) {
	ObjectTree *self = (ObjectTree*)s;
	Fl_Tree_Item *titem = self->first_selected_item();

	if(!titem || !titem->user_data()) return;

	Entity *en = (Entity*)titem->user_data();
	Fl_Text_Buffer *ebuf = self->get_editor_buffer();

	E_RETURN_IF_FAIL(ebuf != NULL);

	char buf[SCRIPT_EDITOR_EVAL_BUFSZ];
	if(en->get_prototype_as_scheme(buf, sizeof(buf))) {
		ebuf->append("\n");
		ebuf->append(buf);
	}
}

static void describe_in_editor_cb(Fl_Widget*, void *s) {
	//ObjectTree *self = (ObjectTree*)s;
}

ObjectTree::ObjectTree(int X, int Y, int W, int H, const char *l) : Fl_Tree(X, Y, W, H, l), editor_buf(NULL), action_menu(NULL) {
	showroot(0);
	item_labelbgcolor(color());

	action_menu = new MenuButton(0, 0, 0, 0);
	action_menu->menu(menu_);
}

void ObjectTree::introspect(const char *service, EdbusConnection *c) {
	clear();

	/* first we start with '/', as root object, since all services implement this one */
	EdbusMessage  m;
	scan_object(c, m, service, "/", this);
	redraw();
}

void ObjectTree::clear(void) {
	Fl_Tree::clear();

	EntityListIt it = entities.begin(), ite = entities.end();
	while(it != ite) {
		delete *it;
		it = entities.erase(it);
	}
}

int ObjectTree::handle(int ev) {
	if(ev == FL_PUSH) {
		const Fl_Tree_Item *clicked = find_clicked();

		if(clicked && clicked->user_data()) {
			/* show prototype as tooltip */
			Entity *e = (Entity*)clicked->user_data();

			if(Fl::event_button() == FL_RIGHT_MOUSE) {
				deselect_all();
				select((Fl_Tree_Item*) clicked, 1);

				/* by default popup() does not call callbacks */
				const MenuItem * m = action_menu->menu()->popup(Fl::event_x(), Fl::event_y());
				if(m && m->callback()) 
					m->do_callback(0, this);
			} else {
				e->get_prototype(tooltip_buf, sizeof(tooltip_buf));

				Fl::belowmouse(this);
				/*
				 * I will never understaind how 'entity_area()' works, as obviously y coordinate always
				 * gets messed up. Here '50' is hardcoded to fix tooltip messed position.
				 */
				Fl_Tooltip::enter_area(this, clicked->x(), clicked->y() - 50, clicked->w(), clicked->h(), tooltip_buf);
			}
		}
	}

	return Fl_Tree::handle(ev);
}
