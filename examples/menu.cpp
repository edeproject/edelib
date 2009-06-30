#include <FL/Fl.H>

#include <edelib/MenuItem.h>
#include <edelib/MenuBar.h>
#include <edelib/Window.h>

edelib::MenuItem menu_[] = {
 {"&File", 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 12, 0},
 {"F&irst item", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 12, 0},
 {"S&econd  item", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 12, 0},
 {"Third item", 0,  0, 0, 128, FL_NORMAL_LABEL, 0, 12, 0},
 {"&Quit", 0x40071,  0, 0, 0, FL_NORMAL_LABEL, 0, 12, 0},
 {0,0,0,0,0,0,0,0,0},
 {"&About", 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 12, 0},
 {"&Help", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 12, 0},
 {0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0}
};

int main(int argc, char **argv) {
	edelib::Window * win = new edelib::Window(215, 144);
	win->begin();
		edelib::MenuBar* o = new edelib::MenuBar(0, 0, 215, 25);
		o->menu(menu_);
	win->end();
	win->show(argc, argv);
	return Fl::run();
}
