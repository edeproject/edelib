/*
 * $Id: String.h 2839 2009-09-28 11:36:20Z karijes $
 *
 * Tooltip support for menus
 * Copyright (c) 2012 edelib authors
 * Copyright 1998-2011 by Bill Spitzak and others.
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

#include <edelib/MenuTooltip.h>
#include <FL/Fl_Tooltip.H>
#include <FL/Fl_Menu_Window.H>
#include <FL/fl_draw.H>

EDELIB_NS_BEGIN

#define MAX_WIDTH 400

class TooltipWin;

Fl_Widget* MenuTooltip::widget_ = 0;
static TooltipWin *window = 0;
static const char *tip;
static int H;
static char recent_tooltip, recursion;

class TooltipWin : public Fl_Menu_Window {
public:
	TooltipWin() : Fl_Menu_Window(0, 0) {
		clear_border();
		set_tooltip_window();
		end();
	}

	void draw();
	void layout();

	/** Shows the tooltip windows only if a tooltip text is available. */
	void show() {
		if (!tip) return;
		Fl_Menu_Window::show();
	}
};

/*
 * The layout() was modified from original Fl_Tooltip, as y coordinate is not remembered any more
 * and mouse y position is used as referent point. This is because remembered y would dump tooltip
 * window around, since originally, the tooltip was shown on the same place as possible; with menus
 * this isn't possible.
 */
void TooltipWin::layout() {
	fl_font(Fl_Tooltip::font(), Fl_Tooltip::size());
	int ww, hh;
	ww = MAX_WIDTH;
	fl_measure(tip, ww, hh, FL_ALIGN_LEFT|FL_ALIGN_WRAP|FL_ALIGN_INSIDE);
	ww += 6; hh += 6;

	int ox = Fl::event_x_root() + 10;
	int oy = Fl::event_y_root() + 10;
	int scr_x, scr_y, scr_w, scr_h;

	Fl::screen_xywh(scr_x, scr_y, scr_w, scr_h);
	if (ox+ww > scr_x+scr_w) ox = scr_x+scr_w - ww;
	if (ox < scr_x) ox = scr_x;
	if (H > 30) {
		oy = Fl::event_y_root()+13;
		if (oy+hh > scr_y+scr_h) oy -= 23+hh;
	} else {
		if (oy+hh > scr_y+scr_h) oy -= (4+hh+H);
	}

	if (oy < scr_y) oy = scr_y;
	resize(ox, oy, ww, hh);
}

void TooltipWin::draw() {
	draw_box(FL_BORDER_BOX, 0, 0, w(), h(), Fl_Tooltip::color());
	fl_color(Fl_Tooltip::textcolor());
	fl_font(Fl_Tooltip::font(), Fl_Tooltip::size());
	fl_draw(tip, 3, 3, w()-6, h()-6, Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_WRAP));
}

static void recent_timeout(void*) {
	recent_tooltip = 0;
}

static void tooltip_timeout(void*) {
	if (recursion) return;

	recursion = 1;
	if (!tip || !*tip) {
		if (window) window->hide();
	} else {
		if (!window) window = new TooltipWin;

		// this cast bypasses the normal Fl_Window label() code:
		((Fl_Widget*)window)->label(tip);
		window->layout();
		window->redraw();
		window->show();
	}

	Fl::remove_timeout(recent_timeout);
	recent_tooltip = 1;
	recursion = 0;
}

void MenuTooltip::current(Fl_Widget* w) {
	MenuTooltip::exit_(0);
	// find the enclosing group with a tooltip:
	Fl_Widget* tw = w;
	for (;;) {
		if (!tw) return;
		if (tw->tooltip()) break;
		tw = tw->parent();
	}

	// act just like Fl_Tooltip::enter_() except we can remember a zero:
	widget_ = w;
}

void MenuTooltip::exit_(Fl_Widget *w) {
	if (!widget_ || (w && w == window)) return;
	widget_ = 0;
	Fl::remove_timeout(tooltip_timeout);
	Fl::remove_timeout(recent_timeout);
	if (window && window->visible()) window->hide();
	if (recent_tooltip) {
		if (Fl::event_state() & FL_BUTTONS) recent_tooltip = 0;
		else Fl::add_timeout(Fl_Tooltip::hoverdelay(), recent_timeout);
	}
}

void MenuTooltip::enter_area(Fl_Widget *wid, int x, int y, int w, int h, const char *t) {
	(void)x;
	(void)w;

	if (recursion) return;
	if (!t || !*t || !Fl_Tooltip::enabled()) {
		exit_(0);
		return;
	}

	// do nothing if it is the same:
	if (wid==widget_ && t==tip) return;
	Fl::remove_timeout(tooltip_timeout);
	Fl::remove_timeout(recent_timeout);

	// remember it:
	widget_ = wid; H = h; tip = t;
	// popup the tooltip immediately if it was recently up:
	if (recent_tooltip) {
		if (window) window->hide();
		Fl::add_timeout(Fl_Tooltip::hoverdelay(), tooltip_timeout);
	} else if (Fl_Tooltip::delay() < .1) {
#ifdef WIN32
		// possible fix for the Windows titlebar, it seems to want the
		// window to be destroyed, moving it messes up the parenting:
		if (window && window->visible()) window->hide();
#endif // WIN32
		tooltip_timeout(0);
	} else {
		if (window && window->visible()) window->hide();
		Fl::add_timeout(Fl_Tooltip::delay(), tooltip_timeout);
	}
}

EDELIB_NS_END
