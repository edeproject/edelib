#include <edelib/ComboBox.h>

#include <fltk/run.h>
#include <fltk/Window.h>
#include <fltk/Choice.h>
#include <fltk/ComboBox.h>
#include <fltk/Button.h>
#include <fltk/Item.h>
#include <edelib/Item.h>

#include <stdio.h>

edelib::ComboBox* gcombo1;
edelib::ComboBox* gcombo2;
edelib::ComboBox* gcombo3;

void close_cb(fltk::Widget*, void* w)
{
	fltk::Window* win = (fltk::Window*)w;
	printf("combo1 %i %s\n", gcombo1->choice(), gcombo1->text());
	printf("combo2 %i %s\n", gcombo2->choice(), gcombo2->text());
	printf("combo3 %i %s\n", gcombo3->choice(), gcombo3->text());
	win->hide();
}

void item_cb(fltk::Widget*, void*)
{
	puts("Item callback");
}

int main (int argc, char **argv) 
{
	fltk::Window* win = new fltk::Window(250, 199, "ComboBox sample");
	win->begin();

	edelib::ComboBox* combo1 = new edelib::ComboBox(10, 25, 230, 25, "Simple:");
	combo1->align(fltk::ALIGN_TOP|fltk::ALIGN_LEFT);
	combo1->type(edelib::COMBOBOX_STATIC);
	combo1->begin();
		new fltk::Item("Item 1 dasdas asdasd asdasd adasdasdasd asdasdas asdasdasdas asdasdad");
		new fltk::Item("Item 2sd ads adsa\n dadasda\n asdad");
		new fltk::Item("Item 3");
		edelib::Item* it1 = new edelib::Item("Item 4");
		it1->offset_x(12);
		it1->callback(item_cb);
	combo1->end();
	gcombo1 = combo1;

	edelib::ComboBox* combo2 = new edelib::ComboBox(10, 75, 230, 25, "Editable:");
	combo2->align(fltk::ALIGN_TOP|fltk::ALIGN_LEFT);
	combo2->type(edelib::COMBOBOX_EDITABLE);
	combo2->begin();
		new fltk::Item("Item 1");
		new fltk::Item("Item 2");
		new fltk::Item("Item 3");
		fltk::Item* it2 = new fltk::Item("Item 4");
		it2->callback(item_cb);
	combo2->end();
	gcombo2 = combo2;
    
	edelib::ComboBox* combo3 = new edelib::ComboBox(10, 125, 230, 25, "Editable (auto completition):");
	combo3->type(edelib::COMBOBOX_AUTOCOMPLETE);
	combo3->align(fltk::ALIGN_TOP|fltk::ALIGN_LEFT);
	combo3->begin();
		new fltk::Item("Item 1");
		new fltk::Item("Item 2");
		new fltk::Item("Item 3");
		fltk::Item* it3 = new fltk::Item("Item 4");
		it3->callback(item_cb);
	combo3->end();
	combo3->choice(3);
	gcombo3 = combo3;

	fltk::Button* close = new fltk::Button(150, 165, 90, 25, "&Close");
	close->callback(close_cb, win);

	win->end();
	win->show();
	return fltk::run();
}
