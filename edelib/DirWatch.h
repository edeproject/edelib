/*
 * $Id$
 *
 * Directory changes notifier
 * Part of edelib.
 * Copyright (c) 2005-2007 EDE Authors.
 *
 * This program is licenced under terms of the 
 * GNU General Public Licence version 2 or newer.
 * See COPYING for details.
 */

#ifndef __DIRWATCH_H__
#define __DIRWATCH_H__

#include "econfig.h"

EDELIB_NAMESPACE {

enum DirWatchFlags {
	DW_CREATE = (1 << 1),       ///< In directory item created
	DW_MODIFY = (1 << 2),       ///< In directory item modified
	DW_ACCESS = (1 << 3),       ///< In directory item is accessed (read, ...)
	DW_RENAME = (1 << 4),       ///< In directory item renamed
	DW_ATTRIB = (1 << 5),       ///< In directory item's attributes changed
	DW_DELETE = (1 << 6)        ///< In directory item is deleted
};

enum DirWatchNotifier {
	DW_NONE    = 0,             ///< None notifier; watching disabled
	DW_DNOTIFY = 1,             ///< dnotify (linux kernel >= 2.4.19)
	DW_INOTIFY = 2              ///< inotify (TODO: version)
};

enum DirWatchReportFlags {
	DW_REPORT_NONE   = -1,      ///< Not figured out what happened
	DW_REPORT_CREATE = 0,       ///< A new item was created
	DW_REPORT_MODIFY,           ///< Existing item was modified
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
 * DirWatch can be used to monitor certain directories for changes of
 * their content (files, subdirectories, etc.). Those changes could be
 * addition, removing, accessing or attribution changes.
 *
 * \note DirWatch <em>can't</em> be used to monitor files directly; 
 * use directory where file belongs to.
 *
 * \note DirWatch for now working only on linux with kernels >= 2.4.19
 *
 * To accept events, you should initialize DirWatch, add appropriate directories
 * with desired notification flags and register callback. That callback will be
 * called when event occured on target directory with appropriate filled parameters.
 * Some of the parameters can be NULL or -1, depending if DirWatch was able to
 * figure out what was happened
 *
 * After initialization, application should go into <em>loop state</em> so it can
 * wait for events.
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
 *   DirWatch::init();  // must be called first
 *
 *   // check creating/deleting
 *   if(!DirWatch::add("/some/directory1", DW_CREATE | DW_DELETE)) 
 *      printf("Fatal, can't monitor /some/directory1\n");
 *
 *   // check any modify
 *   if(!DirWatch::add("/some/directory2", DW_MODIFY))
 *      printf("Fatal, can't monitor /some/directory2\n");
 *
 *   DirWatch::callback(notify_cb);
 *
 *   // go into loop (this should not be used in case of GUI application
 *   // since toolkits already provide similar infinite loops
 *   while(1)
 *      pause();
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
 * When DirWatch is able to figure out what was changed, it will report that; if not 
 * given values will be NULL or -1:
 * \code
 *    // sample callback
 *    void notify_cb(const char* dir, const char* what_changed, int flag) {
 *       // what_changed can be NULL or name of changed file if succeded
 *       // flag can be -1 if failed or one of DirWatchFlags if succeded
 *       ...
 *    }
 * \endcode
 * Final application <b>must</b> check given parameters.
 *
 * When tracking events, at first sight some reports will look odd or maybe wrong; this is
 * related to program/whatever that cause this event. Eg. when you open some file for editing
 * with vim (probably other editors), you will often get DW_CREATE/DW_DELETE events since
 * they create another temporary file, copy old content in it, modify it and move it to
 * file you supposed to edit. Also programs could create some temporary files and delete them.
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
 * After directory is removed from notifier, further events about changes in it
 * will not be delivered.
 *
 * If you want to deliver some data in callback, you can use <em>void*</em> parameter
 * (avid reader will notice similarities with fltk callbacks :P). To remind:
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
 * DirWatch can report how is communicating with kernel (should I notice that for now is
 * for linux only) via notifier() member returning one of DirWatchNotifier elements, so
 * application can decide what to do in case of some, like:
 * \code
 *   if(DirWatch::notifier() == DW_DNOTIFY)
 *     puts("I'm using dnotify");
 *   else if(DirWatch::notifier() == DW_INOTIFY)
 *     puts("I'm using inotify");
 *   else
 *     puts("Unable to watch");
 * \endcode
 *
 * \note inotify support is one of TODOs
 */
class DirWatch {
	private:
		static DirWatch* pinstance;

		DirWatchImpl*    impl;
		DirWatchNotifier dir_notifier;

		void init_once(void);
		DirWatchEntry* scan(int fd);

		const char* figure_changed(DirWatchEntry* e);
		void fill_content(DirWatchEntry* e);

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
		DirWatchNotifier get_notifier(void) { return dir_notifier; }
#endif
		/**
		 * Prepare internal data. This <b>must</b> be called
		 * before any further add()
		 */
		static void init(void);

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

}

#endif
