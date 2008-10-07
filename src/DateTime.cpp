/*
 * $Id$
 *
 * Classes related to date/time and timezones.
 * Part of edelib.
 * Copyright (c) 2005-2007 EDE Authors.
 *
 * This program is licenced under terms of the
 * GNU General Public Licence version 2 or newer.
 * See COPYING for details.
 */


#include <edelib/DateTime.h>
#include <edelib/Nls.h>
#include <edelib/Debug.h>
#include <edelib/Missing.h>

#include <stdlib.h> // free, getenv
#include <string.h> // strdup, strlen
#include <time.h>
#include <stdio.h>

#ifdef HAVE_SETTIMEOFDAY
#include <sys/time.h> // for settimeofday()
#endif

#define YEAR_BASE       1900
#define YEAR_UNIX(y)    (y - YEAR_BASE)
#define YEAR_NORMAL(y)  (y + YEAR_BASE)
#define MONTH_UNIX(m)   (m - 1)
#define MONTH_NORMAL(m) (m + 1)

extern char* tzname[2];

#ifdef HAVE_DAYLIGHT
extern int daylight;
#endif

EDELIB_NS_BEGIN

static const char month_days[2][12] = {
	{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
	{31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31} // Leap year
};

static const short days_in_year[2][12] = {
	{0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 },
	{0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335} // Leap year
};

const char *day_names[] = {
	_("Sunday"),
	_("Monday"),
	_("Tuesday"),
	_("Wednesday"),
	_("Thursday"),
	_("Friday"),
	_("Saturday")
};

const char *month_names[] = {
	_("January"),
	_("February"),
	_("March"),
	_("April"),
	_("May"),
	_("June"),
	_("July"),
	_("August"),
	_("September"),
	_("October"),
	_("November"),
	_("December")
};

static void get_system_time(struct tm* tt, bool local) {
	time_t ct = time(0);

	if (local) {
#ifdef HAVE_LOCALTIME_R
		localtime_r(&ct, tt);
#else
		struct tm* tcurr;
		tcurr = localtime(&ct);
		/*
		 * copy values;
		 * FIXME: really needed since localtime()
		 * returnst pointer to internal static data ?
		 */
		*tt = *tcurr;
#endif
	} else {
#ifdef HAVE_GMTIME_R
		gmtime_r(&ct, tt);
#else
		// same applies as for localtime() above
		struct tm* tcurr;
		tcurr = gmtime(&ct);
		*tt = *tcurr;
#endif
	}
}

TimeZone::TimeZone() : zoneval(0), zcode(0), timeval(0) { 
}

TimeZone::~TimeZone() {
	clear();
}

bool TimeZone::load(const char* zone) {
	if (!zone)
		return false;

	zoneval = strdup(zone);
	/*
	 * localtime() depends on TZ environment variable, and
	 * setting it to desired zone, it will yield correct time
	 * for that zone. 
	 *
	 * So, here is first backed up old TZ value (if exists)
	 * set a new one, then old is returned. Stupid, I know :)
	 */
	char* old = 0;
	char* otz = getenv("TZ");
	if(otz)
		old = strdup(otz);

	struct tm tmp;
	time_t curr = ::time(0);

	edelib_setenv("TZ", zoneval, 1);
	tzset();

	localtime_r(&curr, &tmp);

	const char* c = tzname[tmp.tm_isdst];
	if(c)
		zcode = strdup(c);

	if(old) {
		edelib_setenv("TZ", old, 1);
		free(old);
	} else
		edelib_unsetenv("TZ");

	tzset();

	/* 
	 * Make sure daylight is calculated in too or mktime() will 
	 * wrongly calculate offset. daylight as tzname[] are set by tzset()
	 */
#ifdef HAVE_DAYLIGHT
	tmp.tm_isdst = daylight;
#endif
	timeval = mktime(&tmp);
	return true;
}

bool TimeZone::load_local(void) {
	struct tm tmp;
	time_t curr = ::time(0);
	localtime_r(&curr, &tmp);

	// FIXME: daylight needed here ???
	timeval = mktime(&tmp);
	return true;
}

void TimeZone::clear(void) {
	if(zoneval) {
		free(zoneval);
		zoneval = 0;
	}

	if(zcode) {
		free(zcode);
		zcode = 0;
	}
}

bool TimeZone::set(const char* zone) {
	clear();
	return load(zone);
}


Date::Date() : dayval(0), monthval(0), yearval(0) { }

Date::Date(const Date& d) : dayval(0), monthval(0), yearval(0) {
	if (is_valid(d.year(), d.month(), d.day()))
		set(d.year(), d.month(), d.day());
}

Date& Date::operator=(const Date& d) {
	if (&d != this)
		set(d.year(), d.month(), d.day());
	return *this;
}

Date::~Date() { }

bool Date::set(unsigned short y, unsigned char m, unsigned char d, DateType t) {
	E_ASSERT(Date::is_valid(y, m, d) && "Invalid date");

	if (y == YearNow || m == MonthNow || d == DayNow) {
		struct tm tmp;
		bool do_local;
		if (t == DATE_LOCAL)
			do_local = true;
		else
			do_local = false;

		get_system_time(&tmp, do_local);

		if (y == YearNow)
			y = YEAR_NORMAL(tmp.tm_year);
		if (m == MonthNow)
			m = MONTH_NORMAL(tmp.tm_mon);
		if (d == DayNow)
			d = tmp.tm_mday;
	}

	yearval = y;
	monthval = m;
	dayval = d;

	return true;
}

bool Date::system_set(void) {
	struct tm tmp;

	/*
	 * Make sure to fetch current time so the clock can be synchronised. 
	 * You don't want to know what happens if is not.
	 *
	 * Also, here is fetched _local_ time intentionaly so we don't mess
	 * localtime and UTC deltas.
	 *
	 * FIXME: how then to handle UTC Date and local Time when wanna to set
	 * them??? Oh my!
	 */
	get_system_time(&tmp, true);

	tmp.tm_year = YEAR_UNIX(year());
	tmp.tm_mon  = MONTH_UNIX(month());
	tmp.tm_mday  = day();

	time_t tt = mktime(&tmp);

#ifdef HAVE_SETTIMEOFDAY
	struct timeval tv;
	tv.tv_sec = tt;
	tv.tv_usec = 0; // or settimeofday() will fail
	if (settimeofday(&tv, 0) == -1)
		return false;
	else
		return true;
#elif defined (HAVE_STIME)
	if (stime(&tt) == -1)
		return false;
	else
		return true;
#else
	return false;
#endif
}

// static
bool Date::is_valid(unsigned short y, unsigned char m, unsigned char d) {
	/*
	 * Checks for d == 0 || y == 0 are not done since
	 * YearNow == 0 and DayNow == 0.
	 * FIXME: Resolve this better ?
	 */
	if (m == 0)
		return false;

	if (m > Date::MonthNow || d > Date::days_in_month(y, m))
		return false;
	return true;
}

// static
bool Date::leap_year(unsigned short y) {
	return ((y & 3) == 0 && (y % 100 != 0) || (y % 400 == 0));
}

// static
unsigned char Date::days_in_month(unsigned short y, unsigned char m) {
	E_ASSERT(m <= 31);
	return month_days[Date::leap_year(y)][m-1];
}

unsigned char Date::days_in_month() const {
	return Date::days_in_month(yearval, monthval);
}

unsigned char Date::day_of_week() const {
	unsigned char d = ((yearval + day_of_year() + (yearval - 1) / 4 - (yearval - 1) / 100 + (yearval - 1) / 400) % 7);
	if (d == 0)
		d = 7;
	return d;
}

unsigned short Date::day_of_year() const {
	return days_in_year[leap_year()][monthval-1] + dayval;
}

const char* Date::day_name(void) {
	// days are counted from 0
	return day_names[day_of_week()-1];
}

const char* Date::month_name(void) {
	// months are counted from 1
	return month_names[monthval-1];
}

Date& Date::operator++() {
	if(++dayval > days_in_month()) {
		dayval = 1;
		if(++monthval > 12) {
			monthval = 1;
			++yearval;
		}
	}

	E_ASSERT(Date::is_valid(year(), month(), day()) == true);
	return *this;
}

Date Date::operator++(int) {
	Date tmp(*this);
	++(*this);
	return tmp;
}

Date& Date::operator--() {
	if(--dayval < 1) {
		if(--monthval < 1) {
			monthval = 12;

			--yearval;
			// keep UNIX compatible
			if(yearval < 1900)
				yearval = 1900;
		}

		dayval = days_in_month();
	}

	E_ASSERT(Date::is_valid(year(), month(), day()) == true);
	return *this;
}

Date Date::operator--(int) {
	Date tmp(*this);
	--(*this);
	return tmp;
}

Time::Time() : hourval(0), minval(0), secval(0) { }

Time::~Time() {}

// static
bool Time::is_valid(unsigned char h, unsigned char m, unsigned char s) {
	if(h <= 23 && m <= 59 && s <= 59)
		return true;
	return false;
}

Time::Time(const Time& t) : hourval(0), minval(0), secval(0) {
	if(Time::is_valid(t.hour(), t.minute(), t.second()) == false)
		return;

	hourval = t.hour();
	minval = t.minute();
	secval = t.second();
}

Time& Time::operator=(const Time& t) {
	if(&t == this)
		return *this;

	E_ASSERT(Time::is_valid(t.hour(), t.minute(), t.second()) == true);

	hourval = t.hour();
	minval = t.minute();
	secval = t.second();
	return *this;
}

void Time::set(unsigned char h, unsigned char m, unsigned char s) {
	E_ASSERT(Time::is_valid(h, m, s));

	hourval = h;
	minval = m;
	secval = s;
}

void Time::set_now(void) {
	struct tm tmp;
	// FIXME: allow UTC/Local option
	get_system_time(&tmp, true);
	hourval = tmp.tm_hour;
	minval  = tmp.tm_min;
	// for leap seconds we can have 61; ignore this and set it to 59
	secval  = (tmp.tm_sec > 59 ? 59 : tmp.tm_sec);
}

bool Time::system_set(void) {
	struct tm tmp;

	/*
	 * Make sure to fetch current time so the clock can be synchronised. 
	 * You don't want to know what happens if is not.
	 *
	 * Also, here is fetched _local_ time intentionaly so we don't mess
	 * localtime and UTC deltas.
	 *
	 * FIXME: how then to handle UTC Date and local Time when wanna to set
	 * them??? Oh my!
	 *
	 * NOTE: milliseconds are not used
	 */
	get_system_time(&tmp, true);

	tmp.tm_hour = hourval;
	tmp.tm_min  = minval;
	tmp.tm_sec  = secval;

	time_t tt = mktime(&tmp);

#ifdef HAVE_SETTIMEOFDAY
	struct timeval tv;
	tv.tv_sec = tt;
	tv.tv_usec = 0; // or settimeofday() will fail
	if (settimeofday(&tv, 0) == -1)
		return false;
	else
		return true;
#elif defined (HAVE_STIME)
	if (stime(&tt) == -1)
		return false;
	else
		return true;
#else
	return false;
#endif
}

Time& Time::operator++() {
	if(++secval > 59) {
		secval = 0;

		if(++minval > 59) {
			minval = 0;

			if(++hourval > 23)
				hourval = 0;
		}
	}

	E_ASSERT(Time::is_valid(hour(), minute(), second()) == true);
	return *this;
}

Time Time::operator++(int) {
	Time tmp(*this);
	++(*this);
	return tmp;
}

Time& Time::operator--() {
	// hourval, minval and secval are unsigned
	if(secval == 0) {
		secval = 59;
		if(minval == 0) {
			minval = 59;
			if(hourval == 0)
				hourval = 23;
			else
				hourval--;
		} else
			minval--;
	} else
		secval--;

	return *this;
}

Time Time::operator--(int) {
	Time tmp(*this);
	--(*this);
	return tmp;
}

EDELIB_NS_END

