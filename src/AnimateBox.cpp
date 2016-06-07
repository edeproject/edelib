/*
 * $Id: String.h 2839 2009-09-28 11:36:20Z karijes $
 *
 * Widget with animation support
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

#include <edelib/AnimateBox.h>
#include <edelib/Debug.h>
#include <FL/Fl_Shared_Image.H>
#include <FL/Fl.H>

/* some flags for determining how to behave, but also to keep them in single place */
#define FLAG_ANIMATING           (1 << 1)
#define FLAG_ANIMATING_SHOW_HIDE (1 << 2)

EDELIB_NS_BEGIN

typedef list<Fl_Image*> ImageList;
typedef list<Fl_Image*>::iterator ImageListIt;

/* simple hack to get Fl_Image without Fl_Shared_Image object */
struct MySharedImage : public Fl_Shared_Image {
	Fl_Image *plain_image() { return image_; }
};

static void show_next_image(void *b) {
	AnimateBox *box = (AnimateBox*)b;
	box->next_image();
	Fl::repeat_timeout(box->rate(), show_next_image, b);
}

AnimateBox::~AnimateBox() {
	ImageListIt it = images.begin(), ite = images.end();
	for(; it != ite; ++it) {
		delete *it;
		/* null-ify so Fl_Shared_Image doesn't delete it twice */
		*it = 0;
	}
}

bool AnimateBox::append(Fl_Image *img) {
	E_RETURN_VAL_IF_FAIL(img != NULL, false);
	images.push_back(img);
	curr_image = images.begin();
	return true;
}

bool AnimateBox::append(const char *path) {
	MySharedImage *img = (MySharedImage*)Fl_Shared_Image::get(path);
	E_RETURN_VAL_IF_FAIL(img != NULL, false);
	
	return append(img->plain_image());
}

void AnimateBox::next_image(void) {
	if(images.size() < 1) return;

	if(curr_image != images.end())
		++curr_image;
	if(curr_image == images.end())
		curr_image = images.begin();

	image(*curr_image);
	redraw();
}

void AnimateBox::start_animation(void) {
	if(flags & FLAG_ANIMATING) return;

	Fl::add_timeout(rate(), show_next_image, this);
	flags |= FLAG_ANIMATING;
	flags |= FLAG_ANIMATING_SHOW_HIDE;
}

void AnimateBox::stop_animation(void) {
	if(!(flags & FLAG_ANIMATING)) return;

	Fl::remove_timeout(show_next_image);
	flags &= ~FLAG_ANIMATING;
}

void AnimateBox::rollover(void) {
	stop_animation();
	curr_image = images.begin();
}

void AnimateBox::animate_on_show(void) {
	flags |= FLAG_ANIMATING_SHOW_HIDE;
}

int AnimateBox::handle(int e) {
	int ret = Fl_Box::handle(e);
	if(!(flags & FLAG_ANIMATING_SHOW_HIDE)) return ret;

	switch(e) {
		case FL_SHOW:
			start_animation();
			E_DEBUG("starting animation\n");
			break;
		case FL_HIDE:
			stop_animation();
			break;
	}

	return ret;
}

EDELIB_NS_END
