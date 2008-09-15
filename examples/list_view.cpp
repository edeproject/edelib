#include <edelib/TableBase.h>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>

#include <stdio.h>

EDELIB_NS_USE

struct RowContent {
	const char *v1;
	const char *v2;
};

static void event_cb(Fl_Widget*, void *data);

/* 
 * This is a simple list view widget to show how TableBase
 * can be (and should be) used
 */
class MyListView : public TableBase {
	private:
		const char *header_labels[2];
		RowContent row_array[100];
		int        row_array_sz;
		int        pushed_row;
	protected:
		void draw_cell(TableContext context, int R, int C, int X, int Y, int W, int H);
	public:
		MyListView(int X, int Y, int W, int H, const char *l = 0) : TableBase(X, Y, W, H, l) {
			header_labels[0] = header_labels[1] = ":P";
			row_array_sz = 0;
			pushed_row = -1;

			// must call this or callback will not be called
			when(FL_WHEN_CHANGED | FL_WHEN_RELEASE);
			callback(event_cb, this);
		}
		
		~MyListView() {}

		void set_header_label(int n, const char *l);
		void resize(int X, int Y, int W, int H);
		void add_row(const char *v1, const char *v2);
		void event_cb2();
};

void MyListView::draw_cell(TableContext context, int R, int C, int X, int Y, int W, int H) {
	const char *l;

	switch(context) {
		case CONTEXT_ROW_HEADER:
		case CONTEXT_RC_RESIZE:
			break;

		// draw column header
		case CONTEXT_COL_HEADER:
			fl_font(FL_HELVETICA | FL_BOLD, 12);
			fl_push_clip(X, Y, W, H);

			if(C == pushed_row)
				fl_draw_box(FL_DOWN_BOX, X, Y, W, H, row_header_color());
			else
				fl_draw_box(FL_UP_BOX, X, Y, W, H, row_header_color());

			fl_color(FL_BLACK);
			
			if(C <= 1 || C >= 0)
				l = header_labels[C];
			else
				l = "<unknown>";

			fl_draw(l, X, Y, W, H, FL_ALIGN_CENTER);
			fl_pop_clip();
			break;
		// draw cells
		case CONTEXT_CELL:
			// background
			fl_push_clip(X, Y, W, H);
			fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, FL_WHITE);
			fl_pop_clip();

			// text
			fl_font(FL_HELVETICA, 12);
			fl_push_clip(X + 3, Y + 3, W - 6, H - 6);
			fl_color(FL_BLACK);

			if(row_array_sz > 0) {
				if(C == 0)
					l = row_array[R].v1;
				else
					l = row_array[R].v2;

				fl_draw(l, X + 3, Y + 3, W - 6, H - 6, FL_ALIGN_LEFT);
			}

			fl_pop_clip();
			break;
		default:
			return;
	}
}

void MyListView::set_header_label(int n, const char *l) {
	if(n > 1 || n < 0)
		return;
	header_labels[n] = l;
}

void MyListView::resize(int X, int Y, int W, int H) {
	TableBase::resize(X, Y, W, H);

	col_width(1, 100);
	col_width(0, W - col_width(1) - 4);
}

void MyListView::add_row(const char *v1, const char *v2) {
	row_array[row_array_sz].v1 = v1;
	row_array[row_array_sz].v2 = v2;
	row_array_sz++;

	rows(row_array_sz);
}

static void event_cb(Fl_Widget*, void *data) {
	MyListView *ml = (MyListView*)data;
	ml->event_cb2();
}

void MyListView::event_cb2() {
	int R = callback_row();
	int C = callback_col();
	TableContext context = callback_context();

	if(context == CONTEXT_RC_RESIZE)
		return;

	if(context == CONTEXT_COL_HEADER) {
		if(Fl::event() == FL_PUSH && (C == 0 || C == 1)) {
			pushed_row = C;
			redraw_cell(context,R,C);
		} else if(Fl::event() == FL_RELEASE && (C == 0 || C == 1)) {
			pushed_row = -1;
			redraw_cell(context,R,C);
		}
	}
}

int main() {
	Fl_Window* win = new Fl_Window(475, 410, "TableBase sample");
	win->begin();
		MyListView* ml = new MyListView(10, 10, 455, 390);
		//ml->box(FL_DOWN_BOX);
		ml->color(FL_WHITE);

		ml->set_header_label(0, "Header 1");
		ml->set_header_label(1, "Header 2");

		ml->col_header(1);
		ml->col_header_height(25);
		ml->col_resize(1);
		ml->cols(2);

		ml->col_width(1, 100);
		ml->col_width(0, ml->w() - ml->col_width(1) - 4);

		ml->add_row("type 1", "val 1");
		ml->add_row("type 2", "val 2");
		ml->add_row("type 3", "val 3");
		ml->add_row("type 4", "val 4");
		ml->add_row("type 5", "val 5");
		ml->add_row("type 6", "val 6");
		ml->add_row("type 7", "val 7");
		ml->add_row("type 8", "val 8");
		ml->add_row("type 9", "val 9");
		ml->add_row("type 10", "val 10");
	win->resizable(ml);
	win->end();
	win->show();
	
	return Fl::run();
}
