#include <edelib/ExpandableGroup.h>
#include <fltk/Window.h>
#include <fltk/run.h>
#include <fltk/Button.h>

int main()
{
	fltk::Window* win = new fltk::Window(480, 345, "Sample with 10 childs");
	win->begin();
	edelib::ExpandableGroup* eg = new edelib::ExpandableGroup(15, 15, 455, 315);
	eg->box(fltk::DOWN_BOX);
	eg->color(fltk::WHITE);
	eg->begin();
		new fltk::Button(0, 0, 45, 45, "ch1");
		new fltk::Button(0, 0, 45, 45, "ch2");
		new fltk::Button(0, 0, 45, 45, "ch3");
		new fltk::Button(0, 0, 45, 45, "ch4");
		new fltk::Button(0, 0, 45, 45, "ch5");
		new fltk::Button(0, 0, 45, 45, "ch6");
		new fltk::Button(0, 0, 45, 45, "ch7");
		new fltk::Button(0, 0, 45, 45, "ch8");
		new fltk::Button(0, 0, 45, 45, "ch9");
		new fltk::Button(0, 0, 45, 45, "ch10");
	eg->end();
	win->resizable(eg);
	win->end();
	win->show();
	return fltk::run();
}
