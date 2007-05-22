
#include <fltk/Window.h>
#include <fltk/MenuBar.h>
#include <fltk/ItemGroup.h>
#include <fltk/Item.h>
#include <fltk/Divider.h>
#include <fltk/run.h>

#include <edelib/Item.h>

void quit_cb(fltk::Widget*, void* w)
{
	fltk::Window* win = (fltk::Window*)w;
	win->hide();
}

int main (int argc, char **argv) 
{
	fltk::Window* win = new fltk::Window(255, 100);
	win->begin();
		fltk::MenuBar* mbar = new fltk::MenuBar(0, 0, 255, 25);
		mbar->begin();
			fltk::ItemGroup* ig1 = new fltk::ItemGroup("&File");
			ig1->begin();
				edelib::Item* it1 = new edelib::Item("&Item1");
				it1->offset_x(12, 20);

				edelib::Item* it2 = new edelib::Item("&Item2 dummy");
				it2->offset_x(5);

				edelib::Item* it3 = new edelib::Item("&Item3 more dummy");
				it3->offset_x(0, 20);

				edelib::Item* it4 = new edelib::Item("&Item4 dummy");
				it4->offset_x(7);

				new fltk::Divider();

				edelib::Item* qq = new edelib::Item("&Quit");
				qq->callback(quit_cb, win);
			ig1->end();

			fltk::ItemGroup* ig2 = new fltk::ItemGroup("&Edit");
			ig2->begin();
				edelib::Item* it5 = new edelib::Item("&Copy");
				it5->offset_x(12);

				edelib::Item* it6 = new edelib::Item("&Paste");
				it6->offset_x(12);

				new fltk::Divider();

				edelib::Item* it7 = new edelib::Item("&Copy 2");
				it7->offset_x(12, 12);

				edelib::Item* it8 = new edelib::Item("&Paste 2");
				it8->offset_x(12, 12);
			ig2->end();
		mbar->end();
		
	win->end();
	win->show();
	return  fltk::run();
}
