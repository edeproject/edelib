
#include <edelib/ComboBox.h>
#include <fltk/run.h>
#include <fltk/Window.h>
#include <fltk/Choice.h>
#include <fltk/Button.h>
#include <fltk/ComboBox.h>
#include <fltk/Item.h>

void close_cb(fltk::Widget*, void* w)
{
	fltk::Window* win = (fltk::Window*)w;
	win->hide();
}

int main (int argc, char **argv) 
{
	fltk::Window* win = new fltk::Window(250, 199, "ComboBox sample");
	win->begin();

	edelib::ComboBox* combo1 = new edelib::ComboBox(10, 25, 230, 25, "Simple:");
	combo1->align(fltk::ALIGN_TOP|fltk::ALIGN_LEFT);
	combo1->begin();
		new fltk::Item("Item 1");
		new fltk::Item("Item 2");
		new fltk::Item("Item 3");
		new fltk::Item("Item 4");
	combo1->end();

	fltk::Choice* combo2 = new fltk::Choice(10, 75, 230, 25, "Editable:");
	combo2->align(fltk::ALIGN_TOP|fltk::ALIGN_LEFT);
	combo2->begin();
		new fltk::Item("Item 1");
		new fltk::Item("Item 2");
		new fltk::Item("Item 3");
		new fltk::Item("Item 4");
	combo2->end();
    
	fltk::ComboBox* combo3 = new fltk::ComboBox(10, 125, 230, 25, "Editable (auto completition):");
	combo3->align(fltk::ALIGN_TOP|fltk::ALIGN_LEFT);
	combo3->begin();
		new fltk::Item("Item 1");
		new fltk::Item("Item 2");
		new fltk::Item("Item 3");
		new fltk::Item("Item 4");
	combo3->end();

	fltk::Button* close = new fltk::Button(150, 165, 90, 25, "&Close");
	close->callback(close_cb, win);

	win->end();
	win->show();
	return fltk::run();
}
