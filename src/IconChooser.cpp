/*
 * $Id$
 *
 * Icon chooser
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

#include <FL/Fl_Double_Window.H>
#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Shared_Image.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/fl_draw.H>

#include <edelib/IconChooser.h>
#include <edelib/IconLoader.h>
#include <edelib/Directory.h>
#include <edelib/ExpandableGroup.h>
#include <edelib/Nls.h>
#include <edelib/Debug.h>
#include <edelib/StrUtil.h>
#include <edelib/Missing.h>

/* max icon sizes */
#define MAX_ICON_W  128
#define MAX_ICON_H  128

#undef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))

EDELIB_NS_BEGIN

typedef list<String> StringList;
typedef list<String>::iterator StringListIt;

class IconBox : public Fl_Button {
private: 
	bool sel;
	char *icon_name;
	String icon_path;
	Fl_Color orig_color;

public:
	IconBox(int x, int y, int w, int h, const char *l = 0);
	~IconBox();

	void set_icon_name_and_path(const String& s);
	const String& get_icon_path(void) const { return icon_path; }

	bool selected(void) { return sel; }
	int handle(int event);
};

IconBox::IconBox(int X, int Y, int W, int H, const char *l) : Fl_Button(X, Y, W, H, l) {
	box(FL_FLAT_BOX);
	orig_color = color();
	icon_name = NULL;
	sel = false;
}

IconBox::~IconBox() {
	if(icon_name) free(icon_name);
}

void IconBox::set_icon_name_and_path(const String& s) {
	char *ptr;
	int W = 0, H = 0, len;

	icon_path.assign(s);

	/* get basename without extension */
	ptr = (char*)strrchr(s.c_str(), E_DIR_SEPARATOR);
	if(ptr && *ptr++) {
		char *e = strrchr(ptr, '.');
		if(e) *e = '\0';
	} else {
		ptr = (char*)_("(unknown)");
	}

	/* keep max icon name 64 bytes */
	icon_name = edelib_strndup(ptr, 64);
	len = edelib_strnlen(icon_name, 64);

	fl_measure(icon_name, W, H);

	if(W > w() && len > 10) {
		/* copy as label so we can modify it */
		copy_label(icon_name);
		ptr = (char*)(label() + 10);

		/* end label string with '...' */
		*ptr = '\0';
		*(ptr - 1) = '.';
		*(ptr - 2) = '.';
		*(ptr - 3) = '.';
	} else {
		/* no need to copy it, use just pointer */
		label(icon_name);
	}

	align(FL_ALIGN_INSIDE);
	tooltip(icon_name);
}

/*
 * FIXME: icon will loose focus if is selected then press OK, which will take focus from it.
 * In this case, nothing will be returned; double-click works as expected.
 */
int IconBox::handle(int event) {
	switch(event) {
		case FL_FOCUS:
			orig_color = color();
			color(selection_color());
			redraw();
			sel = true;
			return 1;
		case FL_UNFOCUS:
			color(orig_color);
			redraw();
			sel = false;
			return 1;
		case FL_PUSH:
			take_focus();
			/* double-click */
			if(Fl::event_clicks())
				do_callback();
			return 1;
		case FL_RELEASE:
			return 1;
		default:
			break;
	}

	return Fl_Button::handle(event);
}

class IconChooser : public Fl_Double_Window {
private:
	String ret, start;
	bool browsed_icon; /* user manually found icon */

	Fl_Button *bbrowse, *bok, *bcancel;
	ExpandableGroup *icon_group;

public:
	IconChooser();

	void load_from_list(StringList &lst);
	bool find_focused(void);

	void load(const char *dir);
	void load(IconSizes sz, IconContext ctx);

	void set_browsed_icon(const char *ic) {
		browsed_icon = true;
		ret = ic;
	}

	String& get_ret(void) { return ret; }
	bool is_browsed_icon(void) { return browsed_icon; }
};

static void cancel_cb(Fl_Widget*, void *w) {
	IconChooser *ic = (IconChooser*)w;
	ic->hide();
}

static void ok_cb(Fl_Widget*, void *w) {
	IconChooser *ic = (IconChooser*)w;
	if(ic->find_focused())
		ic->hide();
}

static void browse_cb(Fl_Widget*, void *w) {
	IconChooser *ic = (IconChooser*)w;

	const char *path = fl_file_chooser(_("Choose icon..."), "*.png\t*.jpg\t*.xpm\t*", NULL);
	if(!path) return;

	ic->set_browsed_icon(path);
	ic->hide(); /* hide dialog */
}

/*
 * This callback is called when is double-clicked on icon box inside icon list; since all childs when
 * are double-clicked get focus automatically, this forwarding to ok_cb(), who checks what child is
 * focused, is valid
 */
void iconbox_cb(Fl_Widget*, void *w) {
	ok_cb(NULL, w);
}

IconChooser::IconChooser() : Fl_Double_Window(355, 305, _("Choose icon")) {
	browsed_icon = false;
	begin();

	icon_group = new ExpandableGroup(10, 10, 335, 250);
	icon_group->box(FL_DOWN_BOX);
	icon_group->color(FL_BACKGROUND2_COLOR);
	icon_group->end();

	/* invisible resizable box */
	Fl_Box* ibox = new Fl_Box(115, 170, 35, 80);
	resizable(ibox);

	bbrowse = new Fl_Button(10, 270, 90, 25, _("&Browse..."));
	bbrowse->callback(browse_cb, this);
	bbrowse->tooltip(_("Manually browse for desired icon"));

	bok = new Fl_Button(160, 270, 90, 25, _("&OK"));
	bok->callback(ok_cb, this);
	bcancel = new Fl_Button(255, 270, 90, 25, _("&Cancel"));
	bcancel->callback(cancel_cb, this);

	end();
	set_modal();
}

void IconChooser::load_from_list(StringList &lst) {
	E_RETURN_IF_FAIL(lst.size() > 0);

	Fl_Shared_Image *img = NULL;
	int imax_w = 0, imax_h = 0, iw, ih;

	StringListIt it = lst.begin(), ite= lst.end();

	/*
	 * lst_info contains coresponding indexes with list<String> so we can deduce what
	 * files to skip (not readable image or dimensions greater than allowed); skippable are marked as 0
	 */
	int *lst_info = new int[lst.size()];
	for(int n = 0; it != ite; ++it, ++n) {
		img = Fl_Shared_Image::get((*it).c_str());
		if(!img) {
			lst_info[n] = 0;
			continue;
		}

		iw = img->w();
		ih = img->h();

		if(iw > MAX_ICON_W || ih > MAX_ICON_H) {
			lst_info[n] = 0;
			continue;
		}

		imax_w = MAX(imax_w, iw);
		imax_h = MAX(imax_h, ih);
		lst_info[n] = 1; 
	}

	/* clear potential content of ExpandableGroup */
	icon_group->clear();

	imax_w = (imax_w < 64) ? 64 : imax_w + 10;
	imax_h = (imax_h < 64) ? 64 : imax_h + 10;

	imax_w += 5;
	imax_h += 5;

	/*
	 * focus_index() is only valid on childs before we show them and that is what we need 
	 * so other childs don't mess it when they are added
	 */
	icon_group->set_visible_focus();
	
	IconBox *preview;
	it = lst.begin();

	for(int n = 0; it != ite; ++it, ++n) {
		img = Fl_Shared_Image::get((*it).c_str());

		if(img && lst_info[n] == 1) {
			preview = new IconBox(0, 0, imax_w, imax_h);
			preview->set_icon_name_and_path(*it);

			/* use background/selection from ExpandableGroup */
			preview->color(icon_group->color());
			preview->selection_color(icon_group->color());

			preview->image(img);
			preview->callback(iconbox_cb, this);
			icon_group->add(preview);

			/* allow window to receive events every 10th icon */
			if((n % 10) == 0) Fl::check();
		}
	}

	delete [] lst_info;
}

bool IconChooser::find_focused(void) {
	IconBox *b;
	for(int n = 0; n < icon_group->children(); n++) {
		b = (IconBox*)icon_group->child(n);

		if(b->selected()) {
			ret.assign(b->get_icon_path());
			return true;
		}
	}

	return false;
}

void IconChooser::load(const char *dir) {
	E_ASSERT(dir != NULL);

	StringList lst;
	if(!dir_list(dir, lst, true))
		return;

	load_from_list(lst);
}

void IconChooser::load(IconSizes sz, IconContext ctx) {
	E_RETURN_IF_FAIL(IconLoader::inited() == true);

	StringList lst;
	const IconTheme *t = IconLoader::theme();

	t->query_icons(lst, sz, ctx);
	load_from_list(lst);
}

String icon_chooser(const char *dir) {
	E_ASSERT(dir != NULL);

	IconChooser ic;
	ic.load(dir);

	ic.show();
	while(ic.visible())
		Fl::wait();

	return ic.get_ret();
}

String icon_chooser(IconSizes sz, IconContext ctx, bool always_full_path, bool *outside_theme_ret) {
	IconChooser ic;
	ic.load(sz, ctx);

	ic.show();
	while(ic.visible())
		Fl::wait();

	if(outside_theme_ret)
		*outside_theme_ret = ic.is_browsed_icon();

	String &ret = ic.get_ret();

	if(!always_full_path && !ic.is_browsed_icon() && ret.length() > 0) {
		char *p, *e;
		p = (char*)strrchr(ret.c_str(), E_DIR_SEPARATOR);

		if(!p || !(*p++))
			p = (char*)ret.c_str();

		/* now remove extension */
		e = (char*)strrchr((const char*)p, '.');
		if(e) *e = '\0';

		return String(p);
	}

	return ret;
}

EDELIB_NS_END
