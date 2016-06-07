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

#ifndef __EDELIB_ANIMATEBOX_H__
#define __EDELIB_ANIMATEBOX_H__

#include "edelib-global.h"
#include <FL/Fl_Box.H>
#include <FL/Fl_Image.H>
#include "List.h"

EDELIB_NS_BEGIN 

/**
 * \class AnimateBox
 * \brief Animate list of images inside box
 *
 * Load a list of images (any type FLTK supports) and display animation in loaded order. The images
 * will be shown as is, without any transition between the images, so for fluid animation, make sure to
 * use larger number of images.
 *
 * Inspired from <a href="http://seriss.com/people/erco/fltk/#Animate">Erco's animate example</a>.
 */
class EDELIB_API AnimateBox : public Fl_Box {
private:
	float                     draw_rate;
	int                       flags;
	list<Fl_Image*>           images;
	list<Fl_Image*>::iterator curr_image;

public:
	/** Constructor. */
	AnimateBox(int X, int Y, int W, int H, const char* l = 0) : Fl_Box(X, Y, W, H, l), draw_rate(0.2), flags(0)
	{ align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE | FL_ALIGN_CLIP); }

	/** Clean loaded images. */
	~AnimateBox();

	/** Append image. */
	bool append(Fl_Image *img);

	/** Load image from given path and add it. If path or image is unreadable, returns false. */
	bool append(const char *path);

	/** Set draw rate (how long to show single image). */
	void rate(float r) { draw_rate = r; }

	/** Get draw rate. */
	float rate(void) { return draw_rate; }

	/** Draw next image. Should not be used directly, as AnimateBox uses it in internally. */
	void next_image(void);

	/** Start animation. */
	void start_animation(void);

	/** Stop animation. */
	void stop_animation(void);

	/** Force animation starts from first frame.*/
	void rollover(void);

	/** Widget will perform animation as soon is shown. */
	void animate_on_show(void);

	/** Event handler. */
	int handle(int e);
};

EDELIB_NS_END
#endif
