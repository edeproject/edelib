/*
 * $Id$
 *
 * Classes related to date/time and timezones.
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

#ifndef __DATETIME_H__
#define __DATETIME_H__

#include "edelib-global.h"

EDELIB_NS_BEGIN

/**
 * \class TimeZone
 * \brief A class for getting time from desired time zone
 */
class EDELIB_API TimeZone {
private:
	char* zoneval;
	char* zcode;
	unsigned long timeval;

	bool load(const char* zone);
	bool load_local(void);
	void clear(void);
public:
	/**
	 * Empty constructor
	 */
	TimeZone();

	/**
	 * Clears internal data
	 */
	~TimeZone();

	/**
	 * Set zone. Previously set zone (and it's data) will be cleared
	 * replacing with the current one
	 */
	bool set(const char* zone);

	/**
	 * Return code from set zone
	 */
	const char* code(void)   { return (zcode ? zcode : "??"); }

	/**
	 * Return zone name
	 */
	const char* zone(void)   { return (zoneval ? zoneval : "Unknown"); }

	/**
	 * Return time from given zone. Time is encoded number (same as time_t type)
	 * so it can be used with various time functions that accepts time_t type
	 */
	unsigned long time(void) { return timeval; }
};

/**
 * \enum DateType
 * \brief Types of date settable via Date::set()
 */
enum DateType {
	DATE_LOCAL = 0,   ///< use local date
	DATE_UTC          ///< use UTC date
};

/**
 * \class Date
 * \brief A class for date manipulation.
 *
 * This class represents a calendar date. It has ability to read
 * system date or to use and manipulate with filled year/month/day
 * values.
 *
 * Setting date is via set() member, and it accept year/month/day
 * form. This is sample:
 * \code
 *   Date mydate;
 *   mydate.set(2006, 1, 1);
 *   // following will be true
 *   mydate.day() == 1;
 *   mydate.month() == 1;
 *   mydate.year() == 1;
 * \endcode
 *
 * set() can also accept abbreviations for months or shorthands for
 * current year/month/day (in this case will consult system date)
 * \code
 *   ...
 *   mydate.set(2006, Date::Jun, 10);
 *   // or use curren date
 *   mydate.set(Date::YearNow, Date::MonthNow, Date::DayNow);
 * \endcode
 *
 * set() will check input, and in case of bad date (either abnormal input 
 * values or setting feb/29 in non leap year) will trigger assert.
 *
 * There is a one catch using abbreviations and valid date. For example,
 * setting:
 * \code
 *   mydate.set(0,1,0)
 * \endcode
 * is perfectly valid, since Date::YearNow and Date::DayNow enumerations
 * are zero; in this case, function will behave as it was set:
 * \code
 *   mydate.set(Date::YearNow, Date::Jan, Date::DayNow);
 * \endcode
 *
 * Date contains operators for comparrisons, so this can be done with those:
 * \code
 *   Date d1,d2;
 *   d1.set(2007, 1, 1);
 *   d2.set(2007, 2, 1);
 *
 *   d1 != d2;   // will be true
 *   d1 <= d2;   // also true
 *
 *   d2 = d1;    // assign d1 values
 *   d1 == d2;   // true
 *   ...
 * \endcode
 */
class EDELIB_API Date {
private:
	unsigned char dayval;
	unsigned char monthval;
	unsigned short yearval;

public:
	/**
	 * \enum Month
	 * \brief Abbreviated months
	 */
	enum Month { 
		Jan = 1,   ///< January
		Feb,       ///< February
		Mar,       ///< March
		Apr,       ///< April
		May,       ///< May 
		Jun,       ///< June
		Jul,       ///< July
		Aug,       ///< August
		Sep,       ///< September
		Oct,       ///< October
		Nov,       ///< November
		Dec,       ///< December
		MonthNow   ///< Current system month
	};

	/**
	 * \enum Day
	 * \brief Current day
	 */
	enum Day {
		DayNow = 0  ///< Current system date
	};

	/**
	 * \enum Year
	 * \brief Current year
	 */
	enum Year { 
		YearNow = 0 ///< Current system year
	};

	/**
	 * Construct empty date. All walues will be zero. This is
	 * not valid, so is_valid() member will return false.
	 */
	Date();

	/**
	 * Construct date via previously declared value
	 */
	Date(const Date& d);

	/**
	 * Assign previously declared value to this. Current content
	 * will be replaced
	 */
	Date& operator=(const Date& d);

	/**
	 * Clear internal data
	 */
	~Date();

	/**
	 * Fill internal data. Data should be valid date (including leap year)
	 * or assertion will be triggered.
	 *
	 * Input can be either in form of numbers or abbreviations for month.
	 *
	 * \todo it can accept set(0,1,0); leave this or... (possible bugs ?)
	 *
	 * \return todo
	 * \param y fill with year value
	 * \param m fill with month value; should be 1 <= m <= 12
	 * \param d fill with day value; should be 1 <= d <= 30 (or 31 for months this days number
	 * or 28 if is non-leap year)
	 * \param t fill with localized or UTC date values
	 */
	bool set(unsigned short y, unsigned char m, unsigned char d, DateType t = DATE_LOCAL);

	/**
	 * Tries to set system date with current values. This function
	 * will try to call settimeofday() and if not exists, it will fallback to
	 * stime(). If both functions are not present, it will return false.
	 *
	 * Since these functions requires priviledged user, if they are present
	 * but user is not allow to set date, this function will return false too.
	 * Contrary this, it will return true if able to set date.
	 *
	 * \todo This should be probably a static function
	 */
	bool system_set(void);

	/** Check internal year value if is leap year */
	bool leap_year(void) const { return leap_year(yearval); }

	/** Return internal day value */
	unsigned char  day(void) const    { return dayval; }
	/** Return internal month value */
	unsigned char  month(void) const  { return monthval; }
	/** Return internal year value */
	unsigned short year(void) const   { return yearval; }

	/**
	 * Return day name for internal day value, eg. Monday/Tuesday/...
	 * Returned data <b>must not</b> be modified since is pointing
	 * to static data. You should copy it if you need to modify it.
	 */
	const char*    day_name(void);

	/**
	 * Return month name for internal month value, eg. January/February/...
	 * Returned data <b>must not</b> be modified, since applies as for
	 * day_name().
	 */
	const char*    month_name(void);


	/** Return number of days of internal year/month value */
	unsigned char  days_in_month() const; 
	/** 
	 * Return day in week (1..7) calculated from internal values.
	 * \note 
	 *   Values are in Western format, so 1 == Sunday, 2 == Monday, ...
	 */
	unsigned char  day_of_week() const;

	/** Return day of year of currently set date */
	unsigned short day_of_year() const;

	/**
	 * Increase current date by one. First day value is increased, then if
	 * that day is last in month, month is increased, then if that month is
	 * last in year, year is increased.
	 */
	Date& operator++();

	/**
	 * Suffix increment
	 */
	Date operator++(int);

	/**
	 * Reverse from operator++()
	 */
	Date& operator--();

	/**
	 * Suffix decrement
	 */
	Date operator--(int);

	/**
	 * Check if given year is leap year
	 *
	 * \return true if it is or false if not
	 * \param y is year to check
	 */
	static bool leap_year(unsigned short y);

	/**
	 * Return number of days in given year and month. Given data must
	 * be valid date
	 *
	 * \return number of days for given year/month
	 * \param y is year to check
	 * \param m is month to check
	 */
	static unsigned char days_in_month(unsigned short y, unsigned char m); 

	/**
	 * Validate given year/month/day values, including leap year check
	 *
	 * \return true if date is correct, or false if not
	 * \param y is year
	 * \param m is month
	 * \param d is day
	 */
	static bool is_valid(unsigned short y, unsigned char m, unsigned char d);
};

#ifndef SKIP_DOCS
inline bool operator==(const Date& d1, const Date& d2)
{ return (d1.day() == d2.day() && d1.month() == d2.month() && d1.year() == d2.year()); }

inline bool operator>(const Date& d1, const Date& d2) {
	return (d1.year() > d2.year() || (d1.year() == d2.year() && d1.month() > d2.month()) || 
		(d1.year() == d2.year() && d1.month() == d2.month() && d1.day() > d2.day()));
}
			
inline bool operator!=(const Date& d1, const Date& d2) { return !(d1 == d2); }
inline bool operator>=(const Date& d1, const Date& d2) { return (d1 > d2 || d1 == d2); }
inline bool operator<(const Date& d1, const Date& d2)  { return (!(d1 > d2) && (d1 != d2)); }
inline bool operator<=(const Date& d1, const Date& d2) { return (d1 == d2 || d1 < d2); }
#endif

/**
 * \class Time
 * \brief A class for time manipulation.
 *
 * This class allows you to manipulate with time, fetch or set one.
 * Hour can be 0 <= hour < 24. Minutes and seconds  are in 0 <= val < 60.
 *
 * \note Time where hour is 0 is always less than time where hour is 23.
 *
 * \todo Time is missing local/UTC option.
 */
class EDELIB_API Time {
private:
	unsigned char hourval;
	unsigned char minval;
	unsigned char secval;

public:
	/**
	 * Constructor that will set all time members to 0.
	 * This is valid time.
	 */
	Time();

	/**
	 * Construct time via previously declared value
	 */
	Time(const Time& t);

	/**
	 * Assign previously declared value. Current time will be replaced.
	 */
	Time& operator=(const Time& t);

	/**
	 * Destructor
	 */
	~Time();

	/**
	 * Set time values. Values should be valid time, or assertion will be triggered.
	 *
	 * \param h is hour
	 * \param m is minutes
	 * \param s is seconds
	 */
	void set(unsigned char h, unsigned char m, unsigned char s = 0);

	/**
	 * Read system time and fill internal values
	 *
	 * \todo This should be probably a static function
	 */
	void set_now(void);

	/**
	 * Tries to set system time with current values. This function requires
	 * priviledged user. It behaves the same as Date::system_set() (see it's documentation
	 * for detail description).
	 *
	 * \todo This should be probably a static function
	 */
	bool system_set(void);

	/**
	 * Returns hour
	 */
	unsigned char hour(void) const { return hourval; }

	/**
	 * Returns minutes
	 */
	unsigned char minute(void) const  { return minval; }

	/**
	 * Returns seconds
	 */
	unsigned char second(void) const  { return secval; }

	/**
	 * Increase current time by one. First seconds are increased; when
	 * they reach 59, minutes are increased and seconds are set to 0.
	 * The same applies for minutes/hour case. When hour reach for 23, it
	 * is set to 0 (the same behaviour as you see from your digital clock).
	 */
	Time& operator++();

	/**
	 * Suffix increment
	 */
	Time operator++(int);

	/**
	 * Reverse from operator++()
	 */
	Time& operator--();

	/**
	 * Suffix decrement
	 */
	Time operator--(int);

	/**
	 * Check if given parameters can be valid time
	 *
	 * \return true if time is valid
	 * \param h is hour
	 * \param m is minutes
	 * \param s is seconds
	 */
	static bool is_valid(unsigned char h, unsigned char m, unsigned char s);
};

#ifndef SKIP_DOCS
inline bool operator==(const Time& t1, const Time& t2) { 
	return (t1.hour() == t2.hour() && t1.minute() == t2.minute() && t1.second() == t2.second());
}

inline bool operator>(const Time& t1, const Time& t2) {
	return (t1.hour() > t2.hour() ||
		(t1.hour() == t2.hour() && t1.second() > t2.second()) ||
		t1.second() == t2.second());
}

inline bool operator<(const Time& t1, const Time& t2) {
	return (t1.hour() < t2.hour() ||
		(t1.hour() == t2.hour() && t1.second() < t2.second()) ||
		t1.second() == t2.second());
}

inline bool operator!=(const Time& t1, const Time& t2) { return !(t1 == t2); }
inline bool operator>=(const Time& t1, const Time& t2) { return (t1 > t2 || t1 == t2); }
inline bool operator<=(const Time& t1, const Time& t2) { return (t1 == t2 || t1 < t2); }
#endif

EDELIB_NS_END
#endif // __DATETIME_H__
