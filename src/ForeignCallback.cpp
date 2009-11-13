/*
 * $Id$
 *
 * Foreign callbacks
 * Copyright (c) 2005-2009 edelib authors
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
#include <edelib/ForeignCallback.h>
#include <edelib/List.h>
#include <FL/x.H>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>

EDELIB_NS_BEGIN

typedef void (*ForeignCallback)(Fl_Window*, void*);

struct ForeignCallbackInfo {
	unsigned int    hash_id;
	ForeignCallback cb;
	void            *data;
};

typedef list<ForeignCallbackInfo> CallbackList;
typedef list<ForeignCallbackInfo>::iterator CallbackListIt;

static Atom         _XA_EDELIB_FOREIGN_CALLBACK = 0;
static CallbackList callback_list;

/* FIXME: also in Config.cpp */
static unsigned int do_hash(const char *key, int keylen) {
	unsigned hash ;
	int      i;

	for (i = 0, hash = 0; i < keylen ;i++) {
		hash += (long)key[i] ;
		hash += (hash<<10);
		hash ^= (hash>>6) ;
	}

	hash += (hash <<3);
	hash ^= (hash >>11);
	hash += (hash <<15);

	return hash ;
}

static void init_foreign_callback_atom_once(void) {
	if(!_XA_EDELIB_FOREIGN_CALLBACK)
		_XA_EDELIB_FOREIGN_CALLBACK = XInternAtom(fl_display, "_EDELIB_FOREIGN_CALLBACK", False);
}

static void send_client_message(::Window w, Atom a, unsigned int id) {
	XEvent xev;
	long   mask;

	memset(&xev, 0, sizeof(xev));
	xev.type = ClientMessage;
	xev.xclient.window = w;
	xev.xclient.message_type = a;
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = id;
	xev.xclient.data.l[1] = CurrentTime;
	mask = 0;

	XSendEvent(fl_display, w, False, mask, &xev);
}

static void do_callback_by_id(Fl_Window *win, unsigned int id) {
	/* FIXME: lock this somehow */
	CallbackListIt it = callback_list.begin(), it_end = callback_list.end();
	ForeignCallbackInfo *info;

	for(; it != it_end; ++it) {
		info = &(*it);

		/* call it */
		if(id == info->hash_id && info->cb)
			info->cb(win, info->data);
	}
}

static int xevent_handler(int id) {
	if(fl_xevent->type != ClientMessage)
		goto done;

	if(fl_xevent->xclient.message_type != _XA_EDELIB_FOREIGN_CALLBACK)
		goto done;

	for(Fl_Window *win = Fl::first_window(); win; win = Fl::next_window(win)) {
		if(win->type() < FL_WINDOW)
			continue;

		do_callback_by_id(win, fl_xevent->xclient.data.l[0]);
	}

done:
	/* allow events could be processed again if user installs own handler */
	return 0;
}

void foreign_callback_add(Fl_Window *win, const char *id, ForeignCallback cb, void *data) {
	fl_open_display();

	init_foreign_callback_atom_once();

	/* remove it if exists */
	foreign_callback_remove(cb);

	ForeignCallbackInfo fc;
	fc.hash_id = do_hash(id, strlen(id));
	fc.cb = cb;
	fc.data = data;

	/* FIXME: lock this somehow */
	callback_list.push_back(fc);

	/*
	 * Assure the same handler is not added twice since FLTK keeps internal static list of known
	 * handlers per application instance.
	 */
	Fl::remove_handler(xevent_handler);
	Fl::add_handler(xevent_handler);
}

void foreign_callback_remove(ForeignCallback cb) {
	if(callback_list.empty())
		return;

	/* FIXME: lock this somehow */
	CallbackListIt it = callback_list.begin(), it_end = callback_list.end();
	while(it != it_end) {
		if((*it).cb == cb)
			it = callback_list.erase(it);
		else
			++it;
	}
}

void foreign_callback_call(const char *id) {
	fl_open_display();

	init_foreign_callback_atom_once();

	::Window dummy, root, *children = 0;
	unsigned int nchildren, id_hash;

	root = RootWindow(fl_display, fl_screen);
	XQueryTree(fl_display, root, &dummy, &dummy, &children, &nchildren);
	if(!nchildren)
		return;

	id_hash = do_hash(id, strlen(id));

	for(unsigned int i = 0; i < nchildren; i++) {
		if(children[i] != root)
			send_client_message(children[i], _XA_EDELIB_FOREIGN_CALLBACK, id_hash);
	}

	XFree(children);
	XSync(fl_display, False);
}

EDELIB_NS_END
