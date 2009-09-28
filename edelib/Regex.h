/*
 * $Id$
 *
 * Regex class
 * Copyright (c) 2007-2009 edelib authors
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

#ifndef __EDELIB_REGEX_H__
#define __EDELIB_REGEX_H__

#include "List.h"
#include "String.h"

EDELIB_NS_BEGIN

/**
 * \related Regex
 * \enum RegexMode
 * \brief Flags used for compile()
 */
enum RegexMode {
	RX_EXTENDED       = (1 << 1), ///< extended features
	RX_CASELESS       = (1 << 2), ///< ignore cases
	RX_DOLLAR_ENDONLY = (1 << 3), ///< $ not to match newline at end
	RX_DOTALL         = (1 << 4), ///< . matches anything including newline
	RX_MULTILINE      = (1 << 5), ///< ^ and $ match newlines within data
	RX_UNGREEDY       = (1 << 6)  ///< invert greediness of quantifiers
};

/**
 * \related Regex
 * \enum RegexMatchMode
 * \brief Flags used for match()
 */
enum RegexMatchMode {
	RX_MATCH_ANCHORED = (1 << 1), ///< Match only at the first position
	RX_MATCH_NOTBOL   = (1 << 2), ///< Subject is not the beginning of a line
	RX_MATCH_NOTEOL   = (1 << 3), ///< Subject is not the end of a line
	RX_MATCH_NOTEMPTY = (1 << 4)  ///< An empty string is not a valid match
};

#ifndef SKIP_DOCS
struct RegexData;
#endif

/**
 * \class RegexMatch
 * \brief RegexMatch class
 *
 * Data for matched subpatterns. Used in Regex::split().
 */
struct RegexMatch {
	/** position where matched string starts */
	int offset;
	/** length of matched string */
	int length;
};

/**
 * \class Regex
 * \brief Regex class
 *
 * Regex is pattern matching (aka. regular expression) class used to search certain substrings inside 
 * line or text. This class use Perl compatible expressions thanks to PCRE backend.
 *
 * Before doing some matching, you must compile pattern first buy using compile() member.
 *
 * Matching is done via match() member. It will return the number of matched substrings and (if given) 
 * set RegexMatch list with offsets. If fails, -1 will be returned. Here is the sample:
 * that explains it:
 * \code
 *   Regex r;
 *   Regex::MatchVec mv;
 *
 *   r.compile("(de|br)mo");
 *   r.match("some demo string", 0, &mv);
 *
 * \endcode
 *
 * Here, match() will return 2 and MatchVec list will have (5,4) and (5,2) values (where the first value
 * is offset and the second substring length). This explains one important property inherited from PCRE:
 * when grouped patterns are involved and they are found in string, MatchVec first element will be
 * full matched string and substrings will follow.
 */
class EDELIB_API Regex {
private:
	RegexData* data;

	void clear(void);
	E_DISABLE_CLASS_COPY(Regex)
public:
	/** Shortcut for the list of RegexMatch */
	typedef list<RegexMatch> MatchVec;

	/**
	 * Empty constructor
	 */
	Regex();

	/**
	 * Clears all internal data
	 */
	~Regex();

	/**
	 * Compile pattern for matching. This <b>must</b> be called before match/search functions.
	 *
	 * \return true if pattern compiled successfully, or false if not.
	 * \param pattern is matching pattern
	 * \param m is RegexMode type
	 */
	bool compile(const char* pattern, int m = 0);

	/**
	 * Validate if compile() was successfull. This is short way for checking compile() returned value, like:
	 * \code
	 *   Regex r;
	 *   r.compile(...);
	 *   if(!r)
	 *      // die
	 * \endcode
	 */
	operator bool(void) const;

	/**
	 * Search compiled pattern in str and return number of grouped matches. If pattern wasn't
	 * found or some error occured during search, returned value (depending on error) will be less than 0
	 * (see PCRE documentation about returned values).
	 *
	 * \return the number of grouped matches or value less than 0 in case of error
	 * \param str is target string where compiled pattern is searched
	 * \param match_mode is OR-ed RegexMatchMode value 
	 * \param start is starting position on string
	 * \param len is desired length where matching will occur; if given -1, full length will be searched
	 * \param matches is list of matching objects
	 */
	int match(const char* str, int match_mode, int start, int len, MatchVec* matches);

	/**
	 * The same as above match(), but matching will start from the beginning and full string length
	 * will be used
	 */
	int match(const char* str, int match_mode = 0, MatchVec* matches = 0) 
	{ return match(str, match_mode, 0, -1, matches); }

	/**
	 * Split given str and put each of splitted items in list.
	 *
	 * \return 0 if pattern didn't find or size of the list
	 * \param str is data to be splitted
	 * \param ls is list where to put data
	 * \param match_mode is OR-ed RegexMatchMode value
	 */
	int split(const char* str, list<String>& ls, int match_mode = 0);

	/**
	 * Return error in string form. Returned value points to static data and <b>must not</b> be modified or cleared
	 */
	const char* strerror(void) const;
};

EDELIB_NS_END
#endif
