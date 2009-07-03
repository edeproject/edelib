#include <FL/Fl.H>

#include <edelib/MenuItem.h>
#include <edelib/MenuBar.h>
#include <edelib/Window.h>
#include <edelib/IconLoader.h>

edelib::MenuItem menu_[] = {
 {"&File", 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 12, 0},
 {"F&irst item", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 12, 0},
 {"S&econd item", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 12, 0},
 {"Third item", 0,  0, 0, 128, FL_NORMAL_LABEL, 0, 12, 0},
 {"Submenu", 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 12, 0},
 {"item1", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 12, 0},
 {"item2", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 12, 0},
 {0,0,0,0,0,0,0,0,0},
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

		Fl_Image* img1 = (Fl_Image*)edelib::IconLoader::get("document-new", edelib::ICON_SIZE_TINY);
		Fl_Image* img2 = (Fl_Image*)edelib::IconLoader::get("document-save", edelib::ICON_SIZE_SMALL);

		menu_[1].image(img1);
		menu_[2].image(img2);
	win->end();
	win->show(argc, argv);
	return Fl::run();
}
