#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <edelib/ExpandableGroup.h>
#include <edelib/Window.h>

EDELIB_NS_USING(ExpandableGroup)
EDELIB_NS_USING_AS(Window, EWindow)

int main()
{
	EWindow* win = new EWindow(480, 345, "Sample with 10 childs");
	win->begin();
	ExpandableGroup* eg = new ExpandableGroup(15, 15, 455, 315);
	eg->box(FL_DOWN_BOX);
	eg->color(FL_WHITE);
	eg->begin();
		new Fl_Button(0, 0, 45, 45, "ch1");
		new Fl_Button(0, 0, 45, 45, "ch2");
		new Fl_Button(0, 0, 45, 45, "ch3");
		new Fl_Button(0, 0, 45, 45, "ch4");
		new Fl_Button(0, 0, 45, 45, "ch5");
		new Fl_Button(0, 0, 45, 45, "ch6");
		new Fl_Button(0, 0, 45, 45, "ch7");
		new Fl_Button(0, 0, 45, 45, "ch8");
		new Fl_Button(0, 0, 45, 45, "ch9");
		new Fl_Button(0, 0, 45, 45, "ch10");
	eg->end();
	win->resizable(eg);
	win->end();
	win->show();
	return Fl::run();
}
