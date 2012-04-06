#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <edelib/Window.h>
#include <edelib/AnimateBox.h>

EDELIB_NS_USING_AS(Window, EWindow)
EDELIB_NS_USING(AnimateBox)

AnimateBox *b1, *b2;
EWindow *win;

static void close_cb(Fl_Widget*, void *) {
	win->hide();
}

static void play_cb(Fl_Widget*, void *) {
	b1->start_animation();
	b2->start_animation();
}

static void pause_cb(Fl_Widget*, void*) {
	b1->stop_animation();
	b2->stop_animation();
}

static void stop_cb(Fl_Widget*, void*) {
	b1->stop_animation();
	b1->rollover();

	b2->stop_animation();
	b2->rollover();
}

int main(int argc, char **argv) {
	win = new EWindow(270, 225, "Animation demo");
	win->begin();
		b1 = new AnimateBox(10, 11, 120, 110);
		b1->box(FL_THIN_DOWN_BOX);
		b1->append("images/an01.png");
		b1->append("images/an02.png");
		b1->append("images/an03.png");
		b1->append("images/an04.png");
		b1->append("images/an04.png");
		b1->append("images/an03.png");
		b1->append("images/an02.png");
		b1->append("images/an01.png");
		b1->append("images/an01.png");

		b2 = new AnimateBox(140, 10, 120, 110);
		b2->box(FL_THIN_DOWN_BOX);
		b2->append("images/an11.png");
		b2->append("images/an12.png");

		Fl_Button *play = new Fl_Button(135, 135, 40, 40, "@>");
		play->labelsize(20);
		play->callback(play_cb);
 
		Fl_Button *pause = new Fl_Button(177, 135, 40, 40, "@||");
		pause->labelsize(20);
		pause->callback(pause_cb);

		Fl_Button *stop = new Fl_Button(219, 135, 40, 40, "@square");
		stop->labelsize(14);
		stop->callback(stop_cb);

		Fl_Button *close = new Fl_Button(170, 190, 90, 25, "&Close");
		close->callback(close_cb);
	win->end();
	win->show();
	return Fl::run();
}
