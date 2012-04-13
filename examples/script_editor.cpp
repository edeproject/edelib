#include <edelib/SchemeEditor.h>
#include <edelib/Scheme.h>
#include <edelib/Window.h>
#include <edelib/MessageBox.h>
#include <FL/Fl.H>

EDELIB_NS_USING(SchemeEditor)
EDELIB_NS_USING(ask)
EDELIB_NS_USING_AS(Window, EditorWindow)

static char  eval_buf[1024];
EditorWindow *win;

class MyEditor : public SchemeEditor {
private:
	scheme *sc;

public:
	MyEditor(int X, int Y, int W, int H, const char *l = 0);
	~MyEditor();
	void eval_selection(void);
	virtual int handle(int e);
};

static pointer quit_editor(scheme *sc, pointer args) {
	if(ask("You are going to quit editor. Proceed?"))
		win->hide();
	return sc->T;
}
	
MyEditor::MyEditor(int X, int Y, int W, int H, const char *l) : SchemeEditor(X, Y, W, H, l) {
	sc = edelib_scheme_init_raw();
	FILE *fd;

	fd = fopen("../sslib/init.ss", "r");
	if(fd) edelib_scheme_load_file(sc, fd);

	fd = fopen("../sslib/init-2.ss", "r");
	if(fd) edelib_scheme_load_file(sc, fd);

	edelib_scheme_set_input_port_file(sc, stdin);
	edelib_scheme_set_output_port_string(sc, eval_buf, eval_buf + sizeof(eval_buf));
	EDELIB_SCHEME_DEFINE(sc, quit_editor, "quit");
}

MyEditor::~MyEditor() {
	edelib_scheme_deinit(sc);
}

void MyEditor::eval_selection(void) {
	if(!buffer()->selected()) return;

	char *copy = buffer()->selection_text();
	edelib_scheme_load_string(sc, copy);
	free(copy);

	buffer()->append(eval_buf);

	memset(eval_buf, 0, sizeof(eval_buf));
	edelib_scheme_set_output_port_string(sc, eval_buf, eval_buf + sizeof(eval_buf));
}

int MyEditor::handle(int e) {
	int ret = SchemeEditor::handle(e);

	/* catch shortcut before we enter into SchemeEditor */
	if(e == FL_KEYBOARD) {
		if(Fl::event_state(FL_CTRL) && Fl::event_key() == 'j')
			eval_selection();
	}

	return ret;
}

int main() {
	win = new EditorWindow(620, 250, "edelib script repl");
	MyEditor *e = new MyEditor(0, 0, win->w(), win->h());
	e->textsize(12);
	e->buffer()->append(
";; Welcome to simple edelib script editor and REPL. To use it, type some expression\n"
";; and evaluate it. To do so, select some content and press <CTRL>-j keys.\n"
"\n"
"(println \"Hi from edelib-script!\")\n"
"\n"
";; Print something 10 times\n"
"(for i in (range 1 10)\n"
"  (println \"Hi \" i \" times :P\"))\n"
);

	win->end();
	win->resizable(e);
	win->show();
	return Fl::run();
}
