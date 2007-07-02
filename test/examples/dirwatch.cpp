#include <FL/Fl.h>
#include <FL/Fl_Double_Window.h>
#include <FL/Fl_Multi_Browser.h>
#include <FL/Fl_Text_Display.h>
#include <FL/Fl_Text_Buffer.h>
#include <FL/Fl_Button.h>
#include <FL/Fl_File_Chooser.h>

#include <edelib/DirWatch.h>

#include <stdio.h>

Fl_Double_Window* win = 0;
Fl_Multi_Browser* dirlist = 0;
Fl_Text_Buffer* log = 0;

char write_buff[256];

void add_cb(Fl_Widget*, void*) {
	const char* n = fl_dir_chooser("Add directory", ".");
	if(n) {
		dirlist->add(n);
		snprintf(write_buff, 256, "Added %s to be watched\n", n);
		log->append(write_buff);
		//edelib::DirWatch::add(n, edelib::DW_CREATE|edelib::DW_DELETE|edelib::DW_ATTRIB|edelib::DW_RENAME);
		edelib::DirWatch::add(n, edelib::DW_CREATE|edelib::DW_DELETE|edelib::DW_ATTRIB|edelib::DW_RENAME|edelib::DW_MODIFY);
	}
}

void remove_cb(Fl_Widget*, void*) {
	int ln = dirlist->value();
	if(!ln)
		return;
	const char* txt = dirlist->text(ln);
	if(!txt)
		return;
	if(edelib::DirWatch::remove(txt)) {
		snprintf(write_buff, 256, "Removed %s\n", txt);
		dirlist->remove(ln);
	}
	else
		snprintf(write_buff, 256, "!!! Failed to removed %s\n", txt);

	log->append(write_buff);
}

void close_cb(Fl_Widget*, void*) {
	win->hide();
}

void notify_cb(const char* dir, const char* wh, int flag, void* l) {
	Fl_Text_Buffer* tl = (Fl_Text_Buffer*)l;
	if(wh) {
		snprintf(write_buff, 256, "In %s, %s was ", dir, wh);
		tl->append(write_buff);

		if(flag == edelib::DW_REPORT_CREATE)
			tl->append("created\n");
		else if(flag == edelib::DW_REPORT_DELETE)
			tl->append("deleted\n");
		else if(flag == edelib::DW_REPORT_MODIFY)
			tl->append("modified\n");
		else
			tl->append("unknown\n");
	}
	else {
		snprintf(write_buff, 256, "In %s changed (null!!)\n", dir);
		tl->append(write_buff);
	}
}

int main() {
	edelib::DirWatch::init();

	win = new Fl_Double_Window(380, 360, "Directory watch test");
	win->begin();
		dirlist = new Fl_Multi_Browser(10, 32, 360, 175, "Watched directories");
		dirlist->align(FL_ALIGN_TOP_LEFT);

		Fl_Text_Display* td = new Fl_Text_Display(10, 217, 360, 98);
		log = new Fl_Text_Buffer();
		td->buffer(log);

		if(edelib::DirWatch::notifier() == edelib::DW_DNOTIFY)
			log->append("Loaded with dnotify\n");
		else if(edelib::DirWatch::notifier() == edelib::DW_INOTIFY)
			log->append("Loaded with inotify\n");
		else
			log->append("Loaded with none notifier\n");

		Fl_Button* badd = new Fl_Button(80, 325, 90, 25, "&Add");
		badd->callback(add_cb);

		Fl_Button* bremove = new Fl_Button(175, 325, 90, 25, "&Remove");
		bremove->callback(remove_cb);

		Fl_Button* bclose = new Fl_Button(280, 325, 90, 25, "&Close");
		bclose->callback(close_cb);
	win->end();
	win->show();

	edelib::DirWatch::callback(notify_cb, log);

	Fl::run();
	edelib::DirWatch::shutdown();
}

