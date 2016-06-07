
#include <edelib/SevenSeg.h>

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Counter.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Button.H>
#include <FL/fl_show_colormap.H>


Fl_Counter*      counter = 0;
Fl_Value_Slider* slider = 0;
Fl_Button*       color_button = 0;
Fl_Check_Button* chk = 0;
edelib::SevenSeg*        seg = 0;

void change_cb(Fl_Widget*, void*) {
	seg->value((int)counter->value());
}

void color_cb(Fl_Widget*, void*) {
	Fl_Color c = fl_show_colormap(seg->color());
	color_button->color(c);
	seg->color(c);
	seg->redraw();
}

void slide_cb(Fl_Widget*, void*) {
	seg->bar_width((int)slider->value());
}

void deactivate_cb(Fl_Widget*, void*) {
	if(!chk->value())
		seg->activate();
	else
		seg->deactivate();
}

int main(void) {
	Fl_Double_Window* win = new Fl_Double_Window(240, 165, "SevenSeg sample");
	win->begin();
		seg = new edelib::SevenSeg(10, 10, 70, 92);
		seg->box(FL_UP_BOX);
		seg->value(0);

		counter = new Fl_Counter(95, 20, 135, 20, "value");
		counter->type(1);
		counter->step(1);
		counter->minimum(-1);
		counter->maximum(9);
		counter->align(FL_ALIGN_TOP_LEFT);
		counter->callback(change_cb);

		slider = new Fl_Value_Slider(95, 65, 135, 20, "width");
		slider->type(1);
		slider->step(1);
		slider->minimum(2);
		slider->maximum(20);
		slider->value(seg->bar_width());
		slider->align(FL_ALIGN_TOP_LEFT);
		slider->callback(slide_cb);

		color_button = new Fl_Button(205, 95, 25, 25, "color");
		color_button->align(FL_ALIGN_LEFT);
		color_button->callback(color_cb);
		color_button->color(seg->color());

		chk = new Fl_Check_Button(95, 130, 135, 25, "deactivate");
		chk->down_box(FL_DOWN_BOX);
		chk->callback(deactivate_cb);
	win->end();
	win->show();
	return Fl::run();
}
