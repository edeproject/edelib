/*
 * $Id$
 *
 * Directory changes notifier
 * Copyright (c) 2005-2007 edelib authors
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __DIRWATCH_H__
#define __DIRWATCH_H__

#include "edelib-global.h"

EDELIB_NS_BEGIN

/**
 * \enum DirWatchFlags
 * \brief Flags telling to DirWatch what changes to monitor
 */
enum DirWatchFlags {
	DW_CREATE = (1 << 1),       ///< In directory item created
	DW_MODIFY = (1 << 2),       ///< In directory item modified
	DW_ACCESS = (1 << 3),       ///< In directory item is accessed (read, ...)
	DW_RENAME = (1 << 4),       ///< In directory item renamed
	DW_ATTRIB = (1 << 5),       ///< In directory item's attributes changed
	DW_DELETE = (1 << 6)        ///< In directory item is deleted
};

/**
 * \enum DirWatchNotifier
 * \brief Notifier type used by DirWatch
 */
enum DirWatchNotifier {
	DW_NONE = 0,                ///< None notifier; watching disabled
	DW_INOTIFY,                 ///< inotify (linux kernel >= 2.6.13)
	DW_FAM                      ///< FAM/gamin
};

/**
 * \enum DirWatchReportFlags
 * \brief Returned codes for received event
 */
enum DirWatchReportFlags {
	DW_REPORT_NONE   = -1,      ///< Not figured out what happened
	DW_REPORT_CREATE = 0,       ///< A new item was created
	DW_REPORT_MODIFY,           ///< Existing item was modified
	DW_REPORT_RENAME,           ///< Existing item was renamed
	DW_REPORT_DELETE            ///< Existing item was deleted
};

struct DirWatchEntry;
struct DirWatchImpl;

#ifndef SKIP_DOCS
typedef void (DirWatchCallback)(const char* dir, const char* w, int flags, void* data);
#endif

/**
 * \class DirWatch
 * \brief Directory changes notifier
 *
 * DirWatch can be used to monitor certain directories for their content changes
 * (their files). Those changes can be creating, removing, altering or attributes
 * changes.
 *
 * \note DirWatch <em>can't</em> be used to monitor files directly; 
 * use directory where file belongs to.
 *
 * To accept events, you should initialize DirWatch, add appropriate directories
 * with desired notification flags and register callback. That callback will be
 * called when event occured on target directory with appropriate filled parameters.
 * Some of the parameters can be NULL or -1, depending if DirWatch was able to
 * figure out what was happened.
 *
 * After initialization, application should go into loop state so it can listen and 
 * receive events. This is not a problem for GUI applications since they already use event loops.
 *
 * \note DirWatch relies on FLTK loop, and in case of non GUI application, <em>Fl::wait()</em>
 * still has to be used.
 *
 * This is sample:
 * \code
 *   // our callback
 *   void notify_cb(const char* dir, const char* what_changed, int flags, void*) {
 *      if(what_changed && flag != -1)
 *          printf("Content of %s changed: %s was changed with %i flag\n", dir, what_changed, flag);
 *      else
 *          printf("Content of %s changed", dir);
 *   }
 *
 *   // somewhere in the code
 *   DirWatch::init();
 *
 *   // check creating/deleting
 *   if(!DirWatch::add("/some/directory1", DW_CREATE | DW_DELETE)) 
 *      printf("Fatal, can't monitor /some/directory1\n");
 *
 *   // check any modify
 *   if(!DirWatch::add("/some/directory2", DW_MODIFY))
 *      printf("Fatal, can't monitor /some/directory2\n");
 *
 *   // callback called when something was changed
 *   DirWatch::callback(notify_cb);
 *
 *   // go into loop
 *   while(1) {
 *      // here we restart loop each 5 seconds
 *      Fl::wait(5);
 *   }
 *
 *   //... and when application is done, shutdown() will clear allocated data
 *   DirWatch::shutdown();
 * \endcode
 *
 * You <b>must</b> call init() before further calls or assertion will pop up.
 *
 * Events can be reported multiple times, one after one (thus calling callback)
 * depending what DirWatchFlags is selected. For example, selecting <em>DW_CREATE|DW_ATTRIB</em>
 * will call callback probably three times when new file in watched directory is created; one for 
 * creating and other two for attributes changes. This mostly depends on the way how file is created.
 * Final application should be prepared for this.
 *
 * When DirWatch was not able to figure out what was changed, <em>what_changed</em> could be
 * set to NULL or <em>flag</em> could be set -1, like:
 * \code
 *    void notify_cb(const char* dir, const char* what_changed, int flag) {
 *       // what_changed can be NULL or name of changed file if succeded
 *       // flag can be -1 if failed or one of DirWatchFlags if succeded
 *       ...
 *    }
 * \endcode
 *
 * When tracking events, at first sight some reports will look odd or maybe wrong; this is
 * related to program/whatever that cause this event. Eg. when you open some file for editing
 * with vim (probably other editors), you will often get DW_CREATE/DW_DELETE events since
 * vim creates another temporary file, copy old content in it, modify it and move it to
 * file you supposed to edit.
 *
 * Removing registered directories is done with remove(), but given directory name
 * must be equal to one passed to add(), like:
 * \code
 *    DirWatch::add("/home/baz");
 *    // fine, remove it
 *    DirWatch::remove("/home/baz");
 *
 *    // note ending slash, it will not be removed since  does not matches original
 *    DirWatch::remove("/home/baz/");
 * \endcode
 *
 * After directory is removed from notifier, further change events in it will not be delivered.
 *
 * If you want to deliver some data in callback, you can use <em>void*</em> parameter. To remind:
 * \code
 *   void notify_cb(const char* dir, const char* what_changed, int flags, void* d) {
 *      MySampleWidget* w = (MySampleWidget*)d;
 *      w->write_in_widget(dir);
 *   }
 *
 *   MySampleWidget* w = new MySampleWidget(...);
 *   
 *   DirWatch::callback(notify_cb, mywidget);
 * \endcode
 *
 * DirWatch can report what backend it use for notification via notifier() member
 * which will return one of the DirWatchNotifier elements. Then application can choose special
 * case for some backend when is compiled in.
 */
class EDELIB_API DirWatch {
private:
	static DirWatch* pinstance;

	DirWatchImpl*    impl;
	DirWatchNotifier backend_notifier;

	bool init_backend(void);
	/*
	DirWatchEntry* scan(int fd);

	const char* figure_changed(DirWatchEntry* e);
	void fill_content(DirWatchEntry* e);
	*/

	DirWatch(const DirWatch&);
	DirWatch& operator=(DirWatch&);

public:
	/**
	 * Empty constructor
	 */
	DirWatch();

	/**
	 * Clean internal data
	 */
	~DirWatch();

#ifndef SKIP_DOCS
	static DirWatch* instance(void);

	bool add_entry(const char* dir, int flags);
	bool remove_entry(const char* dir);
	bool have_entry(const char* dir);
	void add_callback(DirWatchCallback* cb, void* data);
	void run_callback(int fd);
	DirWatchNotifier get_notifier(void) { return backend_notifier; }
#endif
	/**
	 * Prepare internal data. This <b>must</b> be called
	 * before any further add()
	 */
	static bool init(void);

	/**
	 * Shutdown watcher and clean data. Any further add() call 
	 * after this will trigger assertion.
	 */
	static void shutdown(void);

	/**
	 * Add directory to be watched. Directory <b>must</b> exists
	 * or will be ignored. It will return true if addition was succesfull
	 * or false if not.
	 */
	static bool add(const char* dir, int flags);

	/**
	 * Remove added entry.
	 */
	static bool remove(const char* dir);

	/**
	 * Register callback called when content of one of added directories
	 * was changed.
	 */
	static void callback(DirWatchCallback& cb, void* data = 0);

	/**
	 * Return current notifier used, or DW_NONE if none of them.
	 */
	static DirWatchNotifier notifier(void);
};

EDELIB_NS_END
#endif // __DIRWATCH_H__
