/*
 * $Id$
 *
 * Regex class
 * Part of edelib.
 * Copyright (c) 2000-2007 EDE Authors.
 *
 * This program is licensed under terms of the
 * GNU General Public License version 2 or newer.
 * See COPYING for details.
 */

#ifndef __REGEX_H__
#define __REGEX_H__

#include "econfig.h"
#include "List.h"
#include "String.h"

EDELIB_NS_BEGIN

/**
 * \enum RegexMode
 * \brief Flags used for matching pattern
 */
enum RegexMode {
	RX_BASIC    = (1<<1), ///< basic regex mode (or posix mode)
	RX_EXTENDED = (1<<2), ///< extended mode
	RX_ICASE    = (1<<3), ///< ignore cases
	RX_NEWLINE  = (1<<4)  ///< see newline as start/end (for '^' and '$' characters)
};

/**
 * \enum RegexErrors
 * \brief Error codes from Regex class
 */
enum RegexErrors {
	RX_SUCCESS = 0,   ///< successfull compilation
	RX_EMPTY,         ///< pattern not compiled
	RX_ERR_BADBR,     ///< invalid use of back reference operator
	RX_ERR_BADPAT,    ///< invalid use of pattern operators like group
	RX_ERR_BADRPT,    ///< invalid use of repetition operators such as '*'
	RX_ERR_EBRACE,    ///< un-matched brace
	RX_ERR_EBRACK,    ///< un-mached bracket
	RX_ERR_ECOLLATE,  ///< invalid collating element
	RX_ERR_ECTYPE,    ///< unknown character
	RX_ERR_EESCAPE,   ///< trailing backslash
	RX_ERR_EPAREN,    ///< un-matched parenthesis
	RX_ERR_ERANGE,    ///< invalid use of range operator
	RX_ERR_ESPACE,    ///< out of memory
	RX_ERR_ESUBREG,   ///< invalid backreference to subexpression
	RX_ERR_EEND,      ///< non specific error
	RX_ERR_SEARCH,    ///< re_search internal error
	RX_ERR_UNKNOWN    ///< unknown error
};

#ifndef SKIP_DOCS
struct RegexData;
#endif

/**
 * \class Regex
 * \brief Regular expression class
 *
 * Regex is pattern matching (aka. regular expression) class used
 * to search certain substrings inside line or text. This class
 * use POSIX compatible expressions, either basic or extended one.
 *
 * Regex class supports matching and searching. Matching is testing
 * is compiled pattern is present inside data via match(). It will return
 * true if does or false if not. On other hand, searching (via search())
 * is locating sub-data, returning position where sub-data starts and
 * it's length.
 *
 * Before doing actual matching/searching, you <b>must</b> compile (via compile())
 * searching pattern or calls to match()/search() will yield assertion.
 *
 * This is short sample how to use match():
 * \code
 *   const char* sample = "this is sample text";
 *   Regex r;
 *   if(!r.compile("[a-zA-Z]+"))
 *      // quit...
 *   if(r.match(sample))
 *      puts("Given string found");
 * \endcode
 *
 * And this is how to use search():
 * \code
 *   Regex r;
 *   r.compile("[0-9]+");
 *   int len = 0;
 *   int start = r.search("abc 123 456", len);
 *
 *   // Note: returned position is counted from 0
 *   // so start == 4 and len == 3. 
 *   // Why len is 3? It is because pattern "[0-9]+" does
 *   // not include space character and counting is stopped
 *   // at first characted that is not number.
 *   if(start >= 0)
 *      printf("Located at %i with len %i\n", start, len);
 *   else
 *      printf("Not found\n");
 * \endcode
 *
 * By default Regex use extended matching patterns.
 *
 * You can use few additional modifiers to adopt matching code depending on your needs.
 * For example, if you want to match or search text ignoring character cases, using
 * RX_ICASE will do that, like:
 * \code
 *   r.compile("[a-z]+", RX_EXTENDED | RX_ICASE);
 *   // of course always check if expression was compiled successfully
 *   r.match("fooBazFOObazFoo") // yield true
 * \endcode
 *
 * RX_NEWLINE will see newline character as starting/ending, like:
 * \code
 *   const char* sample = "foo\nbaz"
 *   // match() will return false since '\n' is not seen as endline
 *   r.compile("foo$");
 *   r.match(sample);
 *
 *   // match() will now return true
 *   r.compile("foo$", RX_NEWLINE);
 *   r.match(sample);
 * \endcode
 *
 * With Regex you can also split data using split() (obviously :-P). Splitted
 * data will match compiled pattern, not it's reverse values (like eg. python split()).
 * Let (in sample) extract only words with alphabetic characters:
 * \code
 *   r.compile("[a-z]+", RX_EXTENDED | RX_ICASE);
 *   list<String> data;
 *   list<String>::iterator i;
 *   r.split("this are 1234 SAMPLE 333 ###  words", data);
 *   i = data.begin();
 *   while(i != data.end()) {
 *     printf("%s\n", (*i).c_str());
 *     ++i;
 *   }
 *
 *   // output:
 *   // this
 *   // are
 *   // SAMPLE
 *   // words
 * \endcode
 *
 * If you want to achieve reverse effect where list will contain data not
 * found in pattern, you have to use regex negate ('^'). 
 * Here is simple tokeniser which will split text into words:
 * \code
 *   int tokenise(const char* txt, list<String> l) {
 *       Regex r;
 *       if(!r.compile("[ \t]+"))
 *          return 0;
 *
 *       return r.split(txt, l);
 *   }
 * \endcode
 *
 * Note that Regex will work only on 8-bit characters and UTF-8 (or else)
 * will fail. This will be addressed in the future.
 */
class EDELIB_API Regex {
	private:
		int errcode;
		RegexData* data;

		void clear(void);
		void clear_regs(void);
		void map_errcode(int ret);

		Regex(const Regex&);
		Regex& operator=(const Regex&);

	public:
		/**
		 * Empty constructor
		 */
		Regex();

		/**
		 * Clears all internal data
		 */
		~Regex();

		/**
		 * Compile pattern for matching. This <b>must</b> be called
		 * before match().
		 *
		 * \return true if pattern compiled successfully, or false if not.
		 * \param pattern is matching pattern
		 * \param m is RegexMode type
		 */
		bool compile(const char* pattern, int m = RX_EXTENDED);

		/**
		 * Validate was compile() was successfull. This is short way for compile()
		 * returned value, like:
		 * \code
		 *   Regex r;
		 *   r.compile(...);
		 *   if(!r)
		 *      // die
		 * \endcode
		 */
		operator bool(void) const   { return ((errcode == RX_SUCCESS) ? 1 : 0); }

		/**
		 * Returns true if given string matches compiled pattern (aka. pattern
		 * is found inside string) or false if not.
		 */
		bool match(const char* str);

		/**
		 * Search compiled pattern in str with given size and return position
		 * where matched data starts and it's length. If optional start is
		 * given, searching will be started from that point.
		 *
		 * \return position where searched data starts or -1 if not found or -2 in case of internal error
		 * \param str is data to be searched
		 * \param len is length of searched data
		 * \param matchlen is returned length of matched data
		 * \param start is position to start from
		 */
		int search(const char* str, int len, int& matchlen, int start);

		/**
		 * Same as search(str, strlen(str), matchlen, 0)
		 */
		int search(const char* str, int& matchlen, int start = 0);

		/**
		 * Split given str and put each of splitted items in list.
		 *
		 * \return 0 if pattern didn't find or size of the list
		 * \param str is data to be splitted
		 * \param ls is list where to put data
		 */
		int split(const char* str, list<String>& ls);

		/**
		 * Return current error code
		 */
		int error(void) { return errcode; }

		/**
		 * Interpret parameter and return associated error code. Returned
		 * value points to static data and <b>must not</b> be modified or cleared.
		 */
		static const char* strerror(int code);

		/**
		 * Return error in string form. Returned
		 * value points to static data and <b>must not</b> be modified or cleared.
		 */
		const char* strerror(void) { return Regex::strerror(errcode); }
};

EDELIB_NS_END
#endif
