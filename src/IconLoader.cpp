/*
 * $Id$
 *
 * Icon loader via IconTheme
 * Copyright (c) 2009 edelib authors
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

#include <FL/Fl_Shared_Image.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Window.H>
#include <FL/Fl.H>
#include <FL/Fl_Button.H>

#include <edelib/List.h>
#include <edelib/String.h>
#include <edelib/Debug.h>
#include <edelib/IconLoader.h>

EDELIB_NS_BEGIN

#define NOT_SET(var, key) (!((var) & (key)))

static const char* fallback_icon  = "empty";
IconLoader* IconLoader::pinstance = NULL;

struct IconLoaderItem {
	String      name;
	String      path;
	IconContext context;
	IconSizes   size;
	int         size_wh;

	Fl_Widget*  widget;
};

typedef list<IconLoaderItem*>           Items;
typedef list<IconLoaderItem*>::iterator ItemsIter;

static IconLoaderItem* find_item(Items& items, const char* icon_name, IconSizes sz) {
	ItemsIter it = items.begin(), it_end = items.end();

	for(; it != it_end; ++it) {
		if((*it)->name == icon_name && (*it)->size == sz)
			return (*it);
	}

	return NULL;
}

static IconLoaderItem* get_or_create_item(Items& items, 
										  const char* name, 
										  IconSizes sz, 
										  IconContext ctx, 
										  IconTheme* theme, 
										  Fl_Widget* widg,
										  bool scale_icon)
{
	IconLoaderItem* item = find_item(items, name, sz);
	if(!item) {
		item = new IconLoaderItem;
		item->name = name;
		item->path = theme->find_icon(name, sz, ctx);
		item->size = sz;
		item->size_wh = 0;
		item->context = ctx;
		item->widget = widg;

		/* see if we are going to look scaled version */
		if(scale_icon)
			item->size_wh = (int)sz;

		/* register it */
		items.push_back(item);
	}

	return item;
}

IconLoader::IconLoader() { 
	curr_theme = new IconTheme;
}

IconLoader::~IconLoader() { 
	clear_items();
	delete curr_theme;
}

void IconLoader::clear_items(void) {
	ItemsIter it = items.begin(), it_end = items.end();

	for(; it != it_end; ++it)
		delete *it;
	items.clear();
}

void IconLoader::load_theme(const char* name) {
	curr_theme->load(name);
}

const char* IconLoader::get_icon_path(const char* name, IconSizes sz, IconContext ctx) {
	IconLoaderItem* item;
	
	item = get_or_create_item(items, name, sz, ctx, curr_theme, NULL, false);
	return item->path.c_str();
}

Fl_Shared_Image* IconLoader::get_icon(const char* name, 
									  IconSizes sz, 
									  IconContext ctx, 
									  unsigned long options)
{
	const char* path;
	Fl_Shared_Image* img = NULL;
	/* Fl_Shared_Image::get(xxx, 0, 0) will not resize image */
	int sz_wh = 0;

	if(NOT_SET(options, ICON_LOADER_OPTION_NO_ICON_SCALE))
		sz_wh = (int)sz;

	if(NOT_SET(options, ICON_LOADER_OPTION_NO_ABSOLUTE_PATH)) {
		/* try to directly open it */
		img = Fl_Shared_Image::get(name, sz_wh, sz_wh);
		if(img)
			return img;
	}

	path = get_icon_path(name, sz, ctx);
	img  = Fl_Shared_Image::get(path, sz_wh, sz_wh);

	/* no image, try fallback then */
	if(!img && fallback_icon) {
		path = get_icon_path(fallback_icon, sz, ctx);
		img = Fl_Shared_Image::get(path, sz_wh, sz_wh);
	}

	return img;
}

bool IconLoader::set_icon(const char* name, 
						  Fl_Widget* widget, 
						  IconSizes sz, 
						  IconContext ctx, 
						  unsigned long options)
{
	Fl_Shared_Image* img = NULL;
	IconLoaderItem* item;
	int sz_wh = 0;

	if(NOT_SET(options, ICON_LOADER_OPTION_NO_ICON_SCALE))
		sz_wh = (int)sz;

	/* try to directly open it */
	if(NOT_SET(options, ICON_LOADER_OPTION_NO_ABSOLUTE_PATH))
		img = Fl_Shared_Image::get(name, sz_wh, sz_wh);
	
	/* then go inside icon theme */
	if(!img) {
		item = get_or_create_item(items, name, sz, ctx, curr_theme, widget, true);
		img = Fl_Shared_Image::get(item->path.c_str(), sz_wh, sz_wh);
	}

	/* no image, try fallback then */
	if(!img && fallback_icon) {
		item = get_or_create_item(items, fallback_icon, sz, ctx, curr_theme, widget, true);
		img = Fl_Shared_Image::get(item->path.c_str(), sz_wh, sz_wh);
	}

	/* nothing, bail out */
	if(!img)
		return false;

	widget->image(img);

	if(NOT_SET(options, ICON_LOADER_OPTION_NO_WIDGET_REDRAW))
		widget->redraw();

	return true;
}

void IconLoader::reload_icons(void) {
	ItemsIter it = items.begin(), it_end = items.end();
	IconLoaderItem* item;

	unsigned int removed = 0, reloaded = 0;

	/* iterate over the list and update each icon name with the new path */
	for(; it != it_end; ++it) {
		item = *it;

		/* remove previous icon from Fl_Shared_Image cache */
		if(!item->path.empty()) {
			Fl_Shared_Image* old_img = Fl_Shared_Image::find(item->path.c_str(), item->size_wh, item->size_wh);

			if(old_img) {
				++removed;
				old_img->release();
			}
		}

		item->path = curr_theme->find_icon(item->name.c_str(), item->size, item->context);
		++reloaded;

		/* 
		 * Also update widget icon. If Fl_Shared_Image::get() fails here, image() will
		 * receive NULL and will signal to FLTK that previous image be removed from drawing
		 * surface, drawing nothing. This is desired behaviour.
		 */
		if(item->widget) {
			Fl_Shared_Image* img = Fl_Shared_Image::get(item->path.c_str(), item->size_wh, item->size_wh);
			Fl_Widget* wi = item->widget;

			wi->image(img);
			wi->redraw();
		}
	}

	E_DEBUG(E_STRLOC ": icon theme reload (removed: %i, reloaded: %i, totally tracked: %i icons)\n", 
			removed, reloaded, items.size());
}

void IconLoader::repoll_icons(void) {
	Fl_Shared_Image* img;
	IconLoaderItem* item;
	ItemsIter it = items.begin(), it_end = items.end();

	while(it != it_end) {
		item = *it;

		/* 
		 * when widget get's destroyed, it's image goes too; this is good since we can
		 * check that with Fl_Shared_Image::find() because it will not exists in Fl_Shared_Image cache
		 */
		img = Fl_Shared_Image::find(item->path.c_str(), item->size_wh, item->size_wh);
		if(!img) {
			/* if not exists, then it was deleted; remove it from our list then */
			delete item;
			it = items.erase(it);
		} else {
			++it;
		}
	}
}

/* static */
void IconLoader::init(const char* theme) {
	if(!IconLoader::pinstance)
		IconLoader::pinstance = new IconLoader();

	IconLoader::pinstance->load_theme(theme);
}

/* static */
void IconLoader::shutdown(void) {
	delete IconLoader::pinstance;
	IconLoader::pinstance = NULL;
}

/* static */
bool IconLoader::inited(void) {
	return (IconLoader::pinstance != NULL);
}

/* static */
void IconLoader::reload(const char* theme) {
	IconLoader::pinstance->load_theme(theme);
	IconLoader::pinstance->reload_icons();
}

/* static */
IconLoader* IconLoader::instance() {
	E_ASSERT(IconLoader::pinstance != NULL && "Did you run IconLoader::init() first?");
	return IconLoader::pinstance;
}

/* static */
Fl_Shared_Image* IconLoader::get(const char* name, 
								 IconSizes sz, 	
								 IconContext ctx, 
								 unsigned long options)
{
	return IconLoader::instance()->get_icon(name, sz, ctx, options);
}

/* static */
String IconLoader::get_path(const char* name, IconSizes sz, IconContext ctx) {
	return IconLoader::instance()->get_icon_path(name, sz, ctx);
}

/* static */
bool IconLoader::set(Fl_Widget* widget, 
					 const char* name, 
					 IconSizes sz, 
					 IconContext ctx, 
					 unsigned long options)
{
	return IconLoader::instance()->set_icon(name, widget, sz, ctx, options);
}

/* static */
const IconTheme* IconLoader::theme(void) {
	return IconLoader::instance()->current_theme();
}

/* static */
void IconLoader::set_fallback_icon(const char* name) {
	fallback_icon = name;
}

/* static */
const char* IconLoader::get_fallback_icon(void) {
	return fallback_icon;
}

/* static */
const char** IconLoader::get_builtin_xpm_icon(IconSizes sz) {
#include "icons/unknown-16.xpm"
#include "icons/unknown-22.xpm"
#include "icons/unknown-32.xpm"
#include "icons/unknown-48.xpm"
#include "icons/unknown-64.xpm"
#include "icons/unknown-128.xpm"

	switch(sz) {
		case ICON_SIZE_TINY:
			return unknown_16_xpm;
		case ICON_SIZE_SMALL:
			return unknown_22_xpm;
		case ICON_SIZE_MEDIUM:
			return unknown_32_xpm;
		case ICON_SIZE_LARGE:
			return unknown_48_xpm;
		case ICON_SIZE_HUGE:
			return unknown_64_xpm;
		case ICON_SIZE_ENORMOUS:
			return unknown_128_xpm;
		default:
			E_ASSERT(0 && "Bad IconSizes value");
			/* not reached */
			return 0;
	}

	/* not reached */
	return 0;
}

EDELIB_NS_END
