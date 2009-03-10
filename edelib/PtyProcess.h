/*
 * $Id$
 *
 * This class enables to "chat" with terminal programs synchronously
 * Copyright (c) 2006-2009 edelib authors
 *
 * This file was a part of the KDE project, module kdesu.
 * Copyright (C) 1999,2000 Geert Jansen <jansen@kde.org>
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

#ifndef __PTYPROCESS_H__
#define __PTYPROCESS_H__

#include "edelib-global.h"
#include <sys/types.h>

EDELIB_NS_BEGIN

class PTY;

/**
 * \class PtyProcess
 * \brief Synchronous communication with tty programs
 *
 * PtyProcess provides synchronous communication with tty based programs.
 * The communications channel used is a pseudo tty (as opposed to a pipe)
 * This means that programs which require a terminal will work.
 */
class EDELIB_API PtyProcess {
private:
	int init();
	int setup_tty(int fd);

	PTY *m_pPTY;
	char *m_Inbuf, *m_TTY;

	class PtyProcessPrivate;
	PtyProcessPrivate *d;

	PtyProcess(const PtyProcess&);
	PtyProcess& operator=(const PtyProcess&);

protected:
#ifndef SKIP_DOCS
	char **environment() const;

	bool m_bErase, m_bTerminal;
	int m_Pid, m_Fd;
	char *m_Command, *m_Exit;
#endif

public:
	/**
	 * Empty constructor
	 */
	PtyProcess();

	/**
	 * Clears data
	 */
	virtual ~PtyProcess();

	/**
	 * Forks off and execute a command. The command's standard in and output
	 * are connected to the pseudo tty. They are accessible with readLine
	 * and writeLine.
	 * \param command The command to execute.
	 * \param args The arguments to the command.
	 */
	int exec(const char *command, const char **args);

	/**
	 * Reads a line from the program's standard out. Depending on the @em block
	 * parameter, this call blocks until a single, full line is read.
	 * \param block Block until a full line is read?
	 * \return The output string.
	 */
	char *read_line(bool block=true);

	/**
	 * Writes a line of text to the program's standard in.
	 * \param line The text to write.
	 * \param addNewline Adds a '\n' to the line.
	 */
	void write_line(const char *line, bool addNewline=true);

	/**
	 * Puts back a line of input.
	 * \param line The line to put back.
	 * \param addNewline Adds a '\n' to the line.
	 */
	void unread_line(const char *line, bool addNewline=true);

	/**
	 * Sets the exit string. If a line of program output matches this,
	 * waitForChild() will terminate the program and return.
	 */
	void set_exit_string(char *exit) { m_Exit = exit; }

	/**
	 * Waits for the child to exit
	 */
	int wait_for_child();

	/**
	 * Waits until the pty has cleared the ECHO flag. This is useful
	 * when programs write a password prompt before they disable ECHO.
	 * Disabling it might flush any input that was written.
	 */
	int wait_slave();

	/**
	 * Enables/disables local echo on the pseudo tty.
	 */
	int enable_local_echo(bool enable=true);

	/**
	 * Enables/disables terminal output. Relevant only to some subclasses
	 */
	void set_terminal(bool terminal) { m_bTerminal = terminal; }

	/**
	 * Overwrites the password as soon as it is used. Relevant only to
	 * some subclasses
	 */
	void set_erase(bool erase) { m_bErase = erase; }

	/**
	 * Set additinal environment variables
	 */
	void set_environment( const char **env );

	/**
	 * Returns the filedescriptor of the process
	 */
	inline int fd() const { return m_Fd; }

	/**
	 * Returns the pid of the process
	 */
	inline int pid() const { return m_Pid; }

	/**
	 * Terminate the child process if still running
	 */
	void terminate_child();

	/**
	 * Wait \p ms miliseconds (ie. 1/10th of a second is 100ms),
	 * using \p fd as a filedescriptor to wait on. Returns
	 * select(2)'s result, which is -1 on error, 0 on timeout,
	 * or positive if there is data on one of the selected fd's.
	 *
	 * \p ms must be in the range 0..999 (ie. the maximum wait
	 * duration is 999ms, almost one second).
	 */
	static int wait_ms(int fd,int ms);

	/**
	 * Basic check for the existence of \p pid.
	 * Returns true iff \p pid is an extant process, (one you could kill - see man kill(2) for signal 0).
	 */
	static bool check_pid(pid_t pid);

	/**
	 * Check process exit status for process \p pid.
	 * On error (no child, no exit), return -1.
	 * If child \p pid has exited, return its exit status, (which may be zero).
	 * If child \p has not exited, return -2.
	 */
	enum CheckPidStatus { Error=-1, NotExited=-2, Killed=-3, Crashed=-4 } ;

	/**
	 * Check process exit status for process @p pid.
	 * On error (no child, no exit), return Error (-1).
	 * If child @p pid has exited, return its exit status,
	 * (which may be zero).
	 * If child @p has not exited, return NotExited (-2).
 	 */
	static int check_pid_exited(pid_t pid);

};

EDELIB_NS_END

#endif
