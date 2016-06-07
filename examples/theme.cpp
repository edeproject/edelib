#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Round_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_File_Browser.H>
#include <FL/Fl_Check_Browser.H>
#include <FL/Fl_Progress.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_File_Input.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_Box.H>

#include <edelib/Window.h>
#include <edelib/ThemeLoader.h>
#include <edelib/IconLoader.h>
#include <edelib/Debug.h>

EDELIB_NS_USING_AS(Window, EdeWindow)
EDELIB_NS_USING(ThemeLoader)
EDELIB_NS_USING(IconLoader)
EDELIB_NS_USING(ICON_SIZE_MEDIUM)

static void quit_cb(Fl_Widget*, void*);

static Fl_Menu_Item menu_[] = {
	{"&File", 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
	{"&Quit", 0x40071,	quit_cb, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
	{0,0,0,0,0,0,0,0,0},
	{"&Edit", 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
	{"item 1", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
	{"item 2", 0,  0, 0, 128, FL_NORMAL_LABEL, 0, 14, 0},
	{"item 3", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0}
};

static EdeWindow *win;

static void quit_cb(Fl_Widget*, void*) {
	win->hide();
}

static void load_theme_cb(Fl_Widget*, void*) {
	E_RETURN_IF_FAIL(ThemeLoader::global()->load_with_path("theme-sample"));

	char buf[128];
	if(ThemeLoader::global()->theme()->get_item("ede", "sample", buf, sizeof(buf)))
		E_DEBUG(E_STRLOC ": evaluated => %s\n", buf);
}

int main(int argc, char **argv) {
	ThemeLoader::init();
	IconLoader::init();
	win = new EdeWindow(455, 485, "Theme demo");
	win->begin();
	{ Fl_Button* o = new Fl_Button(355, 450, 90, 25, "Load");
	  o->tooltip("Load predefined theme");
	  o->callback(load_theme_cb);
	} // Fl_Button* o
	{ Fl_Menu_Bar* o = new Fl_Menu_Bar(0, 0, 455, 25);
	  o->menu(menu_);
	} // Fl_Menu_Bar* o
	{ Fl_Round_Button* o = new Fl_Round_Button(5, 285, 90, 25, "round");
	  o->down_box(FL_ROUND_DOWN_BOX);
	} // Fl_Round_Button* o
	{ new Fl_Return_Button(5, 195, 90, 25, "button");
	} // Fl_Return_Button* o
	{ new Fl_Input(160, 196, 285, 25, "input:");
	} // Fl_Input* o
	{ Fl_Output* o = new Fl_Output(160, 225, 285, 25, "output:");
	  o->value("Some output value");
	} // Fl_Output* o
	{ Fl_File_Browser* o = new Fl_File_Browser(5, 30, 210, 125);
	  o->load("/");
	} // Fl_File_Browser* o
	{ Fl_Check_Browser* o = new Fl_Check_Browser(220, 30, 225, 125);
	  o->add("foo");
	  o->add("foo");
	  o->add("foo");
	} // Fl_Check_Browser* o
	{ Fl_Progress* o = new Fl_Progress(5, 160, 210, 25, "progress bar");
	  o->value(50);
	} // Fl_Progress* o
	{ Fl_Slider* o = new Fl_Slider(220, 160, 225, 25);
	  o->type(1);
	  o->align(Fl_Align(FL_ALIGN_TOP_LEFT));
	} // Fl_Slider* o
	{ new Fl_Light_Button(5, 225, 90, 25, "button");
	} // Fl_Light_Button* o
	{ Fl_Check_Button* o = new Fl_Check_Button(5, 255, 90, 25, "check");
	  o->down_box(FL_DOWN_BOX);
	} // Fl_Check_Button* o
	{ Fl_File_Input* o = new Fl_File_Input(160, 256, 285, 35, "file:");
	  o->value("/home/foo/baz/taz.txt");
	} // Fl_File_Input* o
	{ Fl_Text_Editor* o = new Fl_Text_Editor(160, 325, 285, 115, "Text editor");
	  o->buffer(new Fl_Text_Buffer());
	} // Fl_Text_Editor* o
	{ Fl_Box* o = new Fl_Box(25, 328, 80, 80, "image");
	  IconLoader::set(o, "utilities-terminal", ICON_SIZE_MEDIUM);
	} // Fl_Box* o
	win->end();
	win->show(argc, argv);

	Fl::run();
	ThemeLoader::shutdown();
	IconLoader::shutdown();
	return 0;
}
