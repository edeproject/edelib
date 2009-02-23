//
// TableBase -- A table widget
//
// Copyright 2002 by Greg Ercolano.
// Copyright (c) 2004 O'ksi'D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.
//
// Please report all bugs and problems to "erco at seriss dot com".
//
// TODO:
//	  o Auto scroll during dragged selection
//	  o Keyboard navigation (up/down/left/right arrow)
//

#ifndef __TABLEBASE_H__
#define __TABLEBASE_H__

#include "edelib-global.h"

#include <sys/types.h>
#include <string.h>				// memcpy

#ifdef _WIN32
# include <malloc.h>			// WINDOWS: malloc/realloc
#else
# include <stdlib.h>			// UNIX: malloc/realloc
#endif

#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Scrollbar.H>

EDELIB_NS_BEGIN

/**
 * \class TableBase
 * \brief A base class for table widgets
 *
 * TableBase is incarnation of excelent Fl_Table widget, written by Greg Ercolano. 
 * To be useful, it must be subclassed and several virtual functions defined.
 *
 * This widget does not handle the data in the table. draw_cell() must be overridden by
 * a subclass to manage drawing the content of the cells.
 *
 * Drawing (and callbacks, later explained) are done via <i>contexts</i>. Contexts
 * shows what parts should be redrawn or are changed by user when callbacks are used.
 * \see TableContext
 *
 * Here is the simple table implementation:
 * \code
 *   // table callback
 *   void table_cb(Fl_Widget*, void* data) {
 *     MyTable* t = (MyTable*)data;
 *     t->table_callback();
 *   }
 *
 *   class MyTable : public TableBase {
 *   protected:
 *      void draw_cell(TableContext context, int R, int C, int X, int Y, int W, int H) {
 *        switch(context):
 *          case CONTEXT_STARTPAGE:
 *           // When table or parts of the table are about to be redraw. Use it to initialize
 *           // static data such a font selections or to lock a database before accessing. 
 *           // Here R and C will be zero and X, Y, W and H will have table dimensions.
 *           break;
 *          case CONTEXT_ENDPAGE:
 *           // When table has completed being redrawn. Useful for unlocking a database after accessing.
 *           // Here R and C will be zero and X, Y, W and H will have table dimensions.
 *           break;
 *          case CONTEXT_ROW_HEADER: 
 *           // When row header cell needs to be redrawn
 *          case CONTEXT_COL_HEADER: 
 *           // When columnt header cell needs to be redrawn
 *           // Our table has both row and column headers so draw it
 *           fl_push_clip(X, Y, W, H);
 *           fl_draw_box(FL_UP_BOX, X, Y, W, H, color());
 *           fl_color(FL_BLACK);
 *
 *           // draw "demo" label in each header cell
 *           fl_draw("demo", X, Y, W, H, FL_ALIGN_LEFT);
 *           fl_pop_clip();
 *           break;
 *          case CONTEXT_CELL:
 *           // When data in cells needs to be redrawn
 *           // Here, each cells will have borders and contains "foo" label
 *           fl_push_clip(X, Y, W, H);
 *
 *           // background color
 *           fl_color(FL_WHITE);
 *           fl_rectf(X, Y, W, H);
 *
 *           // text
 *           fl_color(FL_BLACK);
 *           fl_draw("foo", X, Y, W, H, FL_ALIGN_CENTER);
 *
 *           // border
 *           fl_color(FL_GRAY);
 *           fl_rect(X, Y, W, H);
 *
 *           fl_pop_clip();
 *           break;
 *          default:
 *           break;
 *      }
 *
 *   public:
 *      MyTable() {
 *        // table used frames, not boxes
 *        box(FL_DOWN_FRAME);
 *        // let we get all events
 *        when(FL_WHEN_CHANGED | FL_WHEN_RELEASED);
 *        // register our callback
 *        callback(table_cb, this);
 *      }
 *
 *      void table_callback() {
 *        // changed row
 *        int R = callback_row();
 *        // changed column
 *        int C = callback_col();
 *        // context
 *        TableContext context = callback_context();
 *
 *        if(context == CONTEXT_ROW_HEADER) {
 *         // clicked on a row header
 *         // excludes resizing
 *        } else if(context == CONTEXT_COL_HEADER) {
 *         // clicked on a column header
 *         // excludes resizing
 *        } else if(context == CONTEXT_CELL) {
 *         // clicked on a cell
 *         // to receive callback for FL_RELEASE events, you must set when(FL_WHEN_RELEASE)
 *        } else if(context == CONTEXT_RC_RESIZE) {
 *         // resized columns or rows interactively or via col_width() and row_height()
 *         // is_interactive_resize() should be used to determine interactive resize
 *         //
 *         // if row is resized, R will have row number and C will be 0
 *         // if column is resized, C will have column number and R will be 0
 *         // 
 *         // to receive resize evenys, you must set when(FL_WHEN_CHANGED)
 *        }
 *      }
 *   };
 * \endcode
 *
 * Since TableBase is inherited from Fl_Group, it can be container for FLTK widgets too. In that
 * case there is no need to use CONTEXT_CELL in draw_cell() and draw widgets: they will be drawn by itself.
 * The only thing you should use is to <i>add()</i> them, as any other widget is addet to Fl_Group.
 */
class EDELIB_API TableBase : public Fl_Group {
public:
	/**
	 * \enum TableContext
	 * \brief What happened in table callback
	 */
	enum TableContext {
		CONTEXT_NONE	   = 0,
		CONTEXT_STARTPAGE  = 0x01,	///< before a page is redrawn
		CONTEXT_ENDPAGE    = 0x02,	///< after a page is redrawn
		CONTEXT_ROW_HEADER = 0x04,	///< in the row header
		CONTEXT_COL_HEADER = 0x08,	///< in the col header
		CONTEXT_CELL	   = 0x10,	///< in one of the cells
		CONTEXT_TABLE	   = 0x20,	///< in the table
		CONTEXT_RC_RESIZE  = 0x40	///< column or row being resized
	};

private:
	int _rows, _cols,			// total rows/cols
		_row_header_w,			// width of row header
		_col_header_h,			// height of column header
		_row_position,			// last row_position set (not necessarily == toprow!)
		_col_position;			// last col_position set (not necessarily == leftcol!)

	char _row_header,			// row header enabled?
		 _col_header,			// col header enabled?
		 _row_resize,			// row resizing enabled?
		 _col_resize;			// col resizing enabled?
	int
		 _row_resize_min,		// row minimum resizing height (default=1)
		 _col_resize_min;		// col minimum resizing width (default=1)

	// OPTIMIZATION: partial row/column redraw variables
	int _redraw_toprow, _redraw_botrow,
		_redraw_leftcol, _redraw_rightcol;

	Fl_Color _row_header_color,
			 _col_header_color;

	int _auto_drag;
	int _selecting;

	// An STL-ish vector without templates
	class IntVector {
		private:
			int *arr;
			unsigned int _size;

			void init() { arr = NULL; _size = 0; }
			void copy(int *newarr, unsigned int newsize) { size(newsize); memcpy(arr, newarr, newsize * sizeof(int)); }
		public:
			IntVector() { init(); }
			~IntVector() { if ( arr ) free(arr); arr = NULL; }
			IntVector(IntVector&o) { init(); copy(o.arr, o._size); }
			IntVector& operator=(IntVector&o) { init(); copy(o.arr, o._size); return(*this); }
			int operator[](int x) const { return(arr[x]); }
			int& operator[](int x) { return(arr[x]); }
			unsigned int size() { return(_size); }
			void size(unsigned int count) {
				if ( count != _size )
				{ arr = (int*)realloc(arr, count * sizeof(int)); _size = count; }
			}
			int pop_back() { int tmp = arr[_size-1]; _size--; return(tmp); }
			void push_back(int val) { unsigned int x = _size; size(_size+1); arr[x] = val; }
			int back() { return(arr[_size-1]); }
	};

	IntVector
		_colwidths,				// column widths in pixels
		_rowheights;			// row heights in pixels

	Fl_Cursor _last_cursor;		// last mouse cursor before changed to 'resize' cursor

	// EVENT CALLBACK DATA
	TableContext _callback_context;		// event context
	int _callback_row, _callback_col;	// event row/col

	// handle() state variables.
	//	  Put here instead of local statics in handle(), so more
	//	  than one TableBase can exist without crosstalk between them.
	//
	int _resizing_col,					// column being dragged
		_resizing_row,					// row being dragged
		_dragging_x,					// starting x position for horiz drag
		_dragging_y,					// starting y position for vert drag
		_last_row;						// last row we FL_PUSH'ed

	// Redraw single cell
	void _redraw_cell(TableContext context, int R, int C);

	void _start_auto_drag();
	void _stop_auto_drag();
	void _auto_drag_cb();
	static void _auto_drag_cb2(void *d);


protected:
#ifndef SKIP_DOCS
	enum ResizeFlag {
		RESIZE_NONE		 = 0,
		RESIZE_COL_LEFT  = 1,
		RESIZE_COL_RIGHT = 2,
		RESIZE_ROW_ABOVE = 3,
		RESIZE_ROW_BELOW = 4
	};

	int table_w, table_h;						// table's virtual size (in pixels)
	int toprow, botrow,							// four corners of viewable table
		leftcol, rightcol;

	// selection
	int current_row, current_col;
	int select_row, select_col;

	// OPTIMIZATION: Precomputed scroll positions for the toprow/leftcol
	int toprow_scrollpos,
		leftcol_scrollpos;

	// Dimensions
	int tix, tiy, tiw, tih,						// data table inner dimension xywh
		tox, toy, tow, toh,						// data table outer dimension xywh
		wix, wiy, wiw, wih;						// widget inner dimension xywh

	Fl_Scroll *table;							// container for child fltk widgets (if any)
	Fl_Scrollbar *vscrollbar,					// vertical scrollbar
				 *hscrollbar;					// horizontal scrollbar

	// Fltk
	int handle(int e);							// fltk handle() override

	// Class maintenance
	void recalc_dimensions();
	void table_resized();						// table resized; recalc
	void table_scrolled();						// table scrolled; recalc
	void get_bounds(TableContext context,		// return x/y/w/h bounds for context
					int &X, int &Y, int &W, int &H);
	void change_cursor(Fl_Cursor newcursor);	// change mouse cursor to some other shape
	TableContext cursor2rowcol(int &R, int &C, ResizeFlag &resizeflag);
												// find r/c given current x/y event
	int find_cell(TableContext context,			// find cell's x/y/w/h given r/c
				 int R, int C, int &X, int &Y, int &W, int &H);
	int row_col_clamp(TableContext context, int &R, int &C);
												// clamp r/c to known universe

	// Called to draw cells
	virtual void draw_cell(TableContext context, int R=0, int C=0, 
						   int X=0, int Y=0, int W=0, int H=0)
		{ }										// overridden by deriving class

	long row_scroll_position(int row);			// find scroll position of row (in pixels)
	long col_scroll_position(int col);			// find scroll position of col (in pixels)

	int is_fltk_container()						// does table contain fltk widgets?
		{ return( Fl_Group::children() > 3 ); } // (ie. more than box and 2 scrollbars?)

	static void scroll_cb(Fl_Widget*,void*);	// h/v scrollbar callback

	void damage_zone(int r1, int c1, int r2, int c2, int r3 = 0, int c3 = 0);

	void redraw_range(int toprow, int botrow, int leftcol, int rightcol) {
		if ( _redraw_toprow == -1 ) {
			// Initialize redraw range
			_redraw_toprow = toprow;
			_redraw_botrow = botrow;
			_redraw_leftcol = leftcol;
			_redraw_rightcol = rightcol;
		} else {
			// Extend redraw range
			if ( toprow < _redraw_toprow ) _redraw_toprow = toprow;
			if ( botrow > _redraw_botrow ) _redraw_botrow = botrow;
			if ( leftcol < _redraw_leftcol ) _redraw_leftcol = leftcol;
			if ( rightcol > _redraw_rightcol ) _redraw_rightcol = rightcol;
		}

		// Indicate partial redraw needed of some cells
		damage(FL_DAMAGE_CHILD);
	}

	// TODO: maybe _redraw_cell() put protected directly?
	void redraw_cell(TableContext context, int R, int C) { _redraw_cell(context, R, C); }
#endif

public:
	/**
	 * The constructor that creates empty table with no rows or columns. 
	 * Resizing of rows or columns is disabled. Header is not drawn too.
	 */
	TableBase(int X, int Y, int W, int H, const char *l=0);

	/**
	 * Destroys the table and associated widgets
	 */
	~TableBase();

	/**
	 * Clears the table
	 */
	virtual void clear() { rows(0); cols(0); }

	/**
	 * Sets a kind of box around the table (default is FL_NO_BOX). Calling this
	 * function will redraw table
	 */
	inline void table_box(Fl_Boxtype val) { table->box(val); table_resized(); }

	/**
	 * Returns the current box type used for the data table
	 */
	inline Fl_Boxtype table_box( void ) { return(table->box()); }

	/**
	 * Sets the number of rows in the table. Table is redrawn
	 */
	virtual void rows(int val);

	/**
	 * Returns the number of rows in the table
	 */
	inline int rows() { return _rows; }

	/**
	 * Sets the number of columns in the table. Table is redraw
	 */
	virtual void cols(int val);

	/**
	 * Returns the number of columns in the table
	 */
	inline int cols() { return _cols; }

	/**
	 * Returns the range of row and column numbers for all the visible and partially visible
	 * cells in the table.
	 *
	 * These values can be used e.g. by draw_cell() during CONTEXT_STARTPAGE to figure out what
	 * cells are about to be redrawn, for the purposes of locking the data from a database befire it's drawn
	 */
	inline void visible_cells(int& r1, int& r2, int& c1, int& c2) { r1 = toprow; r2 = botrow; c1 = leftcol; c2 = rightcol; }

	/**
	 * Returns 1 if row or column is interactively resized or 0 if not
	 */
	int is_interactive_resize() { return (_resizing_row != -1 || _resizing_col != -1); }

	/**
	 * Allow or disallow row resizing. If 1 is used, row will be resized; 0 is for opposite.
	 * Since interactive resizing is done via the row headers, row_header() must also be enabled to allow resizing
	 */
	void row_resize(int flag) { _row_resize = flag; }

	/**
	 * Returns 1 if rows can be resized or 0 if not
	 */
	inline int row_resize() { return _row_resize; }

	/**
	 * Allow or disallow column resizing. If 1 is used, column will be resized; 0 is for opposite.
	 * Since interactive resizing is done via the column headers, col_header() must also be enabled to allow resizing
	 */
	void col_resize(int flag) { _col_resize = flag; }

	/**
	 * Returns 1 if column can be resized or 0 if not
	 */
	inline int col_resize() { return _col_resize; }

	/**
	 * Sets the current column minimum resize value. Must be a value >= 1
	 */
	void col_resize_min(int val) { _col_resize_min = ( val < 1 ) ? 1 : val; }

	/**
	 * Returns the current column minimum resize value
	 */
	inline int col_resize_min() { return _col_resize_min; }

	/**
	 * Sets the current row minimum resize value. Must be a value >= 1
	 */
	void row_resize_min(int val) { _row_resize_min = ( val < 1 ) ? 1 : val; }

	/**
	 * Returns the current row minimum resize value
	 */
	inline int row_resize_min()	{ return _row_resize_min; }

	/**
	 * Enable or disable showing row headers. 1 will enable them and 0 will disable them.
	 * Table will be redrawn after this call
	 */
	void row_header(int flag) { _row_header = flag; table_resized(); redraw(); }

	/**
	 * Returns 1 if row headers are shown or 0 if not
	 */
	inline int row_header() { return _row_header; }

	/**
	 * Enable or disable showing column headers. 1 will enable them and 0 will disable them.
	 * Table will be redrawn after this call
	 */
	void col_header(int flag) { _col_header = flag; table_resized(); redraw(); }

	/**
	 * Returns 1 if column headers are shown or 0 if not
	 */
	inline int col_header() { return(_col_header); }

	/**
	 * Sets the column header heights. Table will be then redrawn 
	 */
	inline void col_header_height(int height) { _col_header_h = height; table_resized(); redraw(); }

	/**
	 * Returns the column header heights
	 */
	inline int col_header_height() { return _col_header_h; }

	/**
	 * Sets the row header heights. Table will be then redrawn 
	 */
	inline void row_header_width(int width) { _row_header_w = width; table_resized(); redraw(); }

	/**
	 * Returns the row header heights
	 */
	inline int row_header_width() { return _row_header_w; }

	/**
	 * Sets the row header color. Table will be then redrawn
	 */
	inline void row_header_color(Fl_Color val) { _row_header_color = val; redraw(); }

	/**
	 * Returns the current row header color
	 */
	inline Fl_Color row_header_color() { return _row_header_color; }

	/**
	 * Sets the column header color. Table will be then redrawn
	 */
	inline void col_header_color(Fl_Color val) { _col_header_color = val; redraw(); }

	/**
	 * Returns the current column header color
	 */
	inline Fl_Color col_header_color() { return _col_header_color; }

	/**
	 * Sets the height of the specified row in pixels and table is redrawn. callback() will be invoked
	 * with CONTEXT_RC_RESIZE if the row's height was actually changed and when() is FL_WHEN_CHANGED
	 */
	void row_height(int row, int height);

	/**
	 * Returns the current height of specified row
	 */
	inline int row_height(int row) { return ((row<0 || row>=(int)_rowheights.size()) ? 0 : _rowheights[row]); }

	/**
	 * Sets the width of the specified column in pixels and table is redrawn. callback() will be invoked
	 * with CONTEXT_RC_RESIZE if the column's width was actually changed and when() is FL_WHEN_CHANGED
	 */
	void col_width(int col, int width);

	/**
	 * Returns the current height of specified column
	 */
	inline int col_width(int col) { return ((col<0 || col>=(int)_colwidths.size()) ? 0 : _colwidths[col]); }

	/**
	 * Sets the height of all rows to the same value. Table is redrawn then
	 */
	void row_height_all(int height) { for ( int r=0; r<rows(); r++ ) row_height(r, height); }

	/**
	 * Sets the width of all columns to the same value. Table is redrawn then
	 */
	void col_width_all(int width) { for ( int c=0; c<cols(); c++ ) col_width(c, width); }

	/**
	 * Sets the table's current row scroll position
	 */
	void row_position(int row);

	/**
	 * Returns the current row scroll position
	 */
	int row_position() { return _row_position; }

	/**
	 * Sets the table's current column scroll position
	 */
	void col_position(int col);

	/**
	 * Returns the current columnscroll position
	 */
	int col_position() { return _col_position; }

	/**
	 * Sets which row should be at the top of the table, scrolling as necessary. Table is redrawn
	 */
	inline void top_row(int row) { row_position(row); }

	/**
	 * Returns the current top row. This row may be partially obscured
	 */
	inline int top_row() { return row_position(); }

	/**
	 * Returns 1 if the cell with the given row and column values is selected
	 */
	int is_selected(int r, int c);

	/**
	 * Returns selected rows and columns. Values will be leftmost/rightmost column
	 * and topmost/bottommost rows
	 *
	 * This function is used to return bounds of multiple selected cells
	 */
	void get_selection(int& s_top, int& s_left, int& s_bottom, int& s_right);

	/**
	 * Select's rows and columns. Values should be leftmost/rightmost column
	 * and topmost/bottommost rows
	 *
	 * This function is used to select multiple cells
	 */
	void set_selection(int s_top, int s_left, int s_bottom, int s_right);

	/**
	 * Selects cell at the given row/column position
	 *
	 * \todo This function should be called select_cell() or similar
	 */
	int move_cursor(int R, int C);

	/**
	 * Resize table. Table is redrawn
	 */
	void resize(int X, int Y, int W, int H);

	/**
	 * Draw the table (called by FLTK)
	 */
	void draw(void);

	/**
	 * Calls Fl_Group::init_sizes(). Table will be redrawn
	 */
	void init_sizes() { table->init_sizes(); table->redraw(); }

	/**
	 * Append widget to the table
	 */
	void add(Fl_Widget& w) { table->add(w); }

	/**
	 * Append widget to the table
	 */
	void add(Fl_Widget* w) { table->add(w); }

	/**
	 * Same as Fl_Group::insert()
	 */
	void insert(Fl_Widget& w, int n) { table->insert(w,n); }

	/**
	 * Same as Fl_Group::insert()
	 */
	void insert(Fl_Widget& w, Fl_Widget* w2) { table->insert(w,w2); }

	/**
	 * Remove a widget from the table
	 */
	void remove(Fl_Widget& w) { table->remove(w); }

	/**
	 * Same as Fl_Group::begin()
	 */
	void begin() { table->begin(); }

	/**
	 * Same as Fl_Group::end()
	 */
	void end() {
		table->end();

		// HACK: Avoid showing Fl_Scroll; seems to erase screen
		//		 causing unnecessary flicker, even if its box() is FL_NO_BOX.
		//
		if ( table->children() > 2 ) 
			table->show();
		else 
			table->hide();

		Fl_Group::current((Fl_Group*)(Fl_Group::parent()));
	}

#ifndef SKIP_DOCS
	Fl_Widget * const *array() { return table->array(); }
	Fl_Widget *child(int n) const { return table->child(n); }
	int children() const { return table->children()-2; }	// -2: skip Fl_Scroll's h/v scrollbar widgets
	int find(const Fl_Widget *w) const { return table->find(w); }
	int find(const Fl_Widget &w) const { return table->find(w); }
#endif

	/**
	 * Returns the current row event occured on. 
	 * This function should be used from user's callback set with callback()
	 */
	int callback_row() { return _callback_row; }

	/**
	 * Returns the current column event occured on. 
	 * This function should be used from user's callback set with callback()
	 */
	int callback_col() { return _callback_col; }

	/**
	 * Returns current TableContext
	 */
	TableContext callback_context() { return _callback_context; }

	/**
	 * Execute user's callback with given TableContext, row and column
	 */
	void do_callback(TableContext context, int row, int col) {
		_callback_context = context;
		_callback_row = row;
		_callback_col = col;
		Fl_Widget::do_callback();
	}
};

EDELIB_NS_END

#endif /*_FL_TABLE_H*/
