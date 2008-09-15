/*
 * $Id$
 *
 * s7 segment number widget
 * Part of edelib.
 * This widget is based on Fl_SevenSeg by Yves Usson.
 * Copyright (c) 2000-2007 EDE Authors.
 *
 * This program is licenced under terms of the
 * GNU General Public Licence version 2 or newer.
 * See COPYING for details.
 */

#include <edelib/SevenSeg.h>
#include <FL/fl_draw.H>
#include <FL/Fl.H>

#define EDGE_GAP 2

EDELIB_NS_BEGIN

SevenSeg::SevenSeg(int X, int Y, int W, int H) : Fl_Widget(X, Y, W, H), digit(0), segwidth(0) {
	color2(FL_BLACK);
	color(FL_RED);
	segwidth = (W - 8)/13;
	segwidth = segwidth < (H - 8)/13 ? (H - 8)/ 13 : segwidth;
	if(segwidth < 1)
		segwidth = 1;
}

SevenSeg::~SevenSeg() {
}

void SevenSeg::value(int v) {
	if(v < 0)
		digit = -1;
	else if(v > 9)
		digit = 9;
	else
		digit = v;
	if(active())
		redraw();
}

void SevenSeg::value(char c) {
	int v = c - '0';
	value(v);
}

void SevenSeg::bar_width(int w) {
	if(w < 2) w = 2;
	if(w > 12) w = 12;
	segwidth = w;
	if(active())
		redraw();
}

void SevenSeg::draw(void) {
	draw_box();

	int X = x() + Fl::box_dx(box());
	int Y = y() + Fl::box_dy(box());
	int W = w() - Fl::box_dw(box());
	int H = h() - Fl::box_dh(box());

	fl_push_clip(X, Y, W, H);

	fl_color(color2());
	fl_rectf(X, Y, W, H);
	if(digit == -1)
		return;

	X += EDGE_GAP;
	Y += EDGE_GAP;
	W -= EDGE_GAP * 2;
	H -= EDGE_GAP * 2;

	unsigned char r, g, b;
	Fl::get_color(color(), r, g, b);
	if(!active()) {
		int val;
		val = 64 + (int)r/4; r = val > 255 ? 255 : val;
		val = 64 + (int)g/4; g = val > 255 ? 255 : val;
		val = 64 + (int)b/4; b = val > 255 ? 255 : val;
	}

	fl_color(r, g, b);
	fl_line_style(FL_SOLID | FL_CAP_ROUND, segwidth);

	switch(digit) {
		case 0:
			draw_seg_a(X, Y, W, H);
			draw_seg_b(X, Y, W, H);
			draw_seg_c(X, Y, W, H);
			draw_seg_d(X, Y, W, H);
			draw_seg_e(X, Y, W, H);
			draw_seg_f(X, Y, W, H);
			break;
		case 1:
			draw_seg_b(X, Y, W, H);
			draw_seg_c(X, Y, W, H);
			break;
		case 2:
			draw_seg_a(X, Y, W, H);
			draw_seg_b(X, Y, W, H);
			draw_seg_g(X, Y, W, H);
			draw_seg_e(X, Y, W, H);
			draw_seg_d(X, Y, W, H);
			break;
		case 3:
			draw_seg_a(X, Y, W, H);
			draw_seg_b(X, Y, W, H);
			draw_seg_c(X, Y, W, H);
			draw_seg_d(X, Y, W, H);
			draw_seg_g(X, Y, W, H);
			break;
		case 4:
			draw_seg_b(X, Y, W, H);
			draw_seg_c(X, Y, W, H);
			draw_seg_f(X, Y, W, H);
			draw_seg_g(X, Y, W, H);
			break;
		case 5:
			draw_seg_a(X, Y, W, H);
			draw_seg_c(X, Y, W, H);
			draw_seg_d(X, Y, W, H);
			draw_seg_f(X, Y, W, H);
			draw_seg_g(X, Y, W, H);
			break;
		case 6:
			draw_seg_a(X, Y, W, H);
			draw_seg_c(X, Y, W, H);
			draw_seg_d(X, Y, W, H);
			draw_seg_e(X, Y, W, H);
			draw_seg_f(X, Y, W, H);
			draw_seg_g(X, Y, W, H);
			break;
		case 7:
			draw_seg_a(X, Y, W, H);
			draw_seg_b(X, Y, W, H);
			draw_seg_c(X, Y, W, H);
			break;
		case 8:
			draw_seg_a(X, Y, W, H);
			draw_seg_b(X, Y, W, H);
			draw_seg_c(X, Y, W, H);
			draw_seg_d(X, Y, W, H);
			draw_seg_e(X, Y, W, H);
			draw_seg_f(X, Y, W, H);
			draw_seg_g(X, Y, W, H);
			break;
		case 9:
			draw_seg_a(X, Y, W, H);
			draw_seg_b(X, Y, W, H);
			draw_seg_c(X, Y, W, H);
			draw_seg_d(X, Y, W, H);
			draw_seg_g(X, Y, W, H);
			draw_seg_f(X, Y, W, H);
			break;
	}

	fl_line_style(FL_SOLID, 0);

	fl_pop_clip();
}

void SevenSeg::draw_seg_a(int X, int Y, int W, int H) {
	int x1 = X + 2 * segwidth + 1;
	int y1 = Y + segwidth;
	int x2 = X + W - 2 * segwidth - 1;
	fl_line(x1, y1, x2, y1);
}

void SevenSeg::draw_seg_b(int X, int Y, int W, int H) {
	int x1 = X + W - segwidth;
	int y1 = Y + (3 * segwidth)/2 + 1;
	int y2 = y1 + H/2 - 2 * segwidth - 1;
	fl_line(x1,y1,x1,y2);
}

void SevenSeg::draw_seg_c(int X, int Y, int W, int H) {
	int x1 = X + W - segwidth;
	int y1 = Y + (H + segwidth)/2 + 1;
	int y2 = y1 + H/2 - 2 * segwidth - 1;
	fl_line(x1,y1,x1,y2);
}

void SevenSeg::draw_seg_d(int X, int Y, int W, int H) {
	int x1 = X + 2 * segwidth + 1;
	int y1 = Y + H - segwidth;
	int x2 = X + W - 2 * segwidth - 1;
	fl_line(x1,y1,x2,y1);
}

void SevenSeg::draw_seg_e(int X, int Y, int W, int H) {
	int x1 = X + segwidth;
	int y1 = Y + (H + segwidth)/2 + 1;
	int y2 = y1 + H/2 - 2 * segwidth - 1;
	fl_line(x1,y1,x1,y2);
}

void SevenSeg::draw_seg_f(int X, int Y, int W, int H) {
	int x1 = X + segwidth;
	int y1 = Y + (3 * segwidth)/2 + 1;
	int y2 = y1 + H/2 - 2 * segwidth - 1;
	fl_line(x1,y1,x1,y2);
}

void SevenSeg::draw_seg_g(int X, int Y, int W, int H) {
	int x1 = X + 2 * segwidth + 1;
	int y1 = Y + (H - segwidth/2)/2;
	int x2 = X + W - 2 * segwidth - 1;
	fl_line(x1,y1,x2,y1);
}

EDELIB_NS_END
