#include <FL/Fl.H>

#include <edelib/MenuItem.h>
#include <edelib/MenuBar.h>
#include <edelib/MenuButton.h>
#include <edelib/Window.h>
#include <edelib/IconLoader.h>

EDELIB_NS_USING(MenuItem)
EDELIB_NS_USING(MenuButton)
EDELIB_NS_USING(MenuBar)
EDELIB_NS_USING(IconLoader)
EDELIB_NS_USING(ICON_SIZE_TINY)
EDELIB_NS_USING(ICON_SIZE_SMALL)
EDELIB_NS_USING_AS(Window, AppWindow)

MenuItem menu_[] = {
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

MenuItem menu_menu[] = {
 {"Some long item without meaning", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 12, 0},
 {"Item 2", 0,  0, 0, 2, FL_NORMAL_LABEL, 0, 12, 0},
 {"Even longer menu item used only for testing aaaa ffff", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 12, 0},
 {0,0,0,0,0,0,0,0,0}
};


int main(int argc, char **argv) {
	AppWindow* win = new AppWindow(215, 144);
	win->begin();
		MenuBar* o = new MenuBar(0, 0, 215, 25);
		o->menu(menu_);

		Fl_Image* img1 = (Fl_Image*)IconLoader::get("document-new", ICON_SIZE_TINY);
		Fl_Image* img2 = (Fl_Image*)IconLoader::get("document-save", ICON_SIZE_SMALL);
		Fl_Image* img3 = (Fl_Image*)IconLoader::get("system-log-out", ICON_SIZE_TINY);

		menu_[1].image(img1);
		menu_[2].image(img2);
		menu_[8].image(img3);

		MenuButton* b = new MenuButton(65, 80, 90, 25, "menu");
		b->menu(menu_menu);

		Fl_Image* img4 = (Fl_Image*)IconLoader::get("folder", ICON_SIZE_TINY);
		menu_menu[0].image(img4);
	win->end();
	win->show(argc, argv);
	return Fl::run();
}
