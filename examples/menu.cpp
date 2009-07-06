#include <FL/Fl.H>

#include <edelib/MenuItem.h>
#include <edelib/MenuBar.h>
#include <edelib/MenuButton.h>
#include <edelib/Window.h>
#include <edelib/IconLoader.h>

edelib::MenuItem menu_[] = {
 {"&File", 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 12, 0},
 {"F&irst item", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 12, 0},
 {"S&econd item", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 12, 0},
 {"Third item ithodasd asdasd asd asdasd asd asdas asdasdasd asdasdsad dasdas asdasdasd", 0,  0, 0, 128, FL_NORMAL_LABEL, 0, 12, 0},
 {"Submenu", 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 12, 0},
 {"&Item 1", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 12, 0},
 {"I&tem 2", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 12, 0},
 {0,0,0,0,0,0,0,0,0},
 {"&Quit", 0x40071,  0, 0, 0, FL_NORMAL_LABEL, 0, 12, 0},
 {0,0,0,0,0,0,0,0,0},
 {"&About", 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 12, 0},
 {"&Help", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 12, 0},
 {0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0}
};

edelib::MenuItem menu_menu[] = {
 {"Some long item without meaning", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 12, 0},
 {"Item 2", 0,  0, 0, 2, FL_NORMAL_LABEL, 0, 12, 0},
 {"Even longer menu item used only for testing aaaa ffff", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 12, 0},
 {0,0,0,0,0,0,0,0,0}
};


int main(int argc, char **argv) {
	edelib::Window * win = new edelib::Window(215, 144);
	win->begin();
		edelib::MenuBar* o = new edelib::MenuBar(0, 0, 215, 25);
		o->menu(menu_);

		Fl_Image* img1 = (Fl_Image*)edelib::IconLoader::get("document-new", edelib::ICON_SIZE_TINY);
		Fl_Image* img2 = (Fl_Image*)edelib::IconLoader::get("document-save", edelib::ICON_SIZE_SMALL);
		Fl_Image* img3 = (Fl_Image*)edelib::IconLoader::get("system-log-out", edelib::ICON_SIZE_TINY);

		menu_[1].image(img1);
		menu_[2].image(img2);
		menu_[8].image(img3);

		edelib::MenuButton* b = new edelib::MenuButton(65, 80, 90, 25, "menu");
		b->menu(menu_menu);

		Fl_Image* img4 = (Fl_Image*)edelib::IconLoader::get("folder", edelib::ICON_SIZE_TINY);
		menu_menu[0].image(img4);
	win->end();
	win->show(argc, argv);
	return Fl::run();
}
