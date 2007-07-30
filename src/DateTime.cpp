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

#include <stdlib.h> // free, getenv, setenv
#include <string.h> // strdup, strlen
#include <time.h>
#include <stdio.h>

#ifdef HAVE_SETTIMEOFDAY
#include <sys/time.h> // for settimeofday()
#endif

// to keep in one place :P
#define YEAR_UNIX(y)    (y - 1900)
#define YEAR_NORMAL(y)  (y + 1900)
#define MONTH_UNIX(m)   (m - 1)
#define MONTH_NORMAL(m) (m + 1)

EDELIB_NS_BEGIN

const char month_days[2][12] = {
	{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
	{31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31} //Leap year
};

const short days_in_year[2][12] = {
	{0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 },
	{0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335} //Leap year
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

void get_system_time(struct tm* tt, bool local) {
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
		struct tm* tcurr;
		// same applies as for localtime() above
		tcurr = gmtime(&ct);
		*tt = *tcurr;
#endif
	}
}

TimeZone::TimeZone() : pathval(0), zoneval(0), tzone(0), timeval(0) { }

TimeZone::~TimeZone() {
	clear();
}

bool TimeZone::load(const char* path, const char* zone) {
	if (!path || !zone)
		return false;

	pathval = strdup(path);
	zoneval = strdup(zone);

	unsigned int len = strlen(pathval) + strlen(zoneval) + 5;
	char* fullpth = new char[len];

	snprintf(fullpth, len, "%s/%s", pathval, zoneval);
	// TODO: file_exists

	delete [] fullpth;

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
	if (otz)
		old = strdup(otz);
	setenv("TZ", zoneval, 1);

	struct tm tmp;
	time_t curr = ::time(0);
	localtime_r(&curr, &tmp);

	if (tzname[0])
		tzone = strdup(tzname[0]);

	if (old) {
		setenv("TZ", old, 1);
		free(old);
	} else
		unsetenv("TZ");

	timeval = mktime(&tmp);
	return true;
}

bool TimeZone::load_local(void) {
	struct tm tmp;
	time_t curr = ::time(0);
	localtime_r(&curr, &tmp);

	timeval = mktime(&tmp);
	return true;
}

void TimeZone::clear(void) {
	if (pathval) {
		free(pathval);
		pathval = 0;
		puts("clear pathval");
	}

	if (zoneval) {
		free(zoneval);
		zoneval = 0;
		puts("clear zoneval");
	}

	if (tzone) {
		free(tzone);
		tzone = 0;
		puts("clear tzone");
	}
}

bool TimeZone::set(const char* path, const char* zone) {
	return load(path, zone);
}

unsigned long TimeZone::time(void) {
	return timeval;
}

const char* TimeZone::get_timezone(void) {
	return tzone;
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
	EASSERT(is_valid(y, m, d) && "Invalid date");

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

	printf("year: %i real: %i my: %i\n", tmp.tm_year, YEAR_UNIX(year()), year());

	time_t tt = mktime(&tmp);

#ifdef HAVE_SETTIMEOFDAY
	struct timeval tv;
	tv.tv_sec = tt;
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

bool Date::is_valid(unsigned short y, unsigned char m, unsigned char d) const {
	/*
	 * Checks for d == 0 || y == 0 are not done since
	 * YearNow == 0 and DayNow == 0.
	 * FIXME: Resolve this better ?
	 */
	if (m == 0)
		return false;

	if (m > Date::MonthNow || d > days_in_month(y, m))
		return false;
	return true;
}

bool Date::leap_year(unsigned short y) const {
	return ((y & 3) == 0 && (y % 100 != 0) || (y % 400 == 0));
}

unsigned char Date::days_in_month(unsigned short y, unsigned char m) const {
	return month_days[leap_year(y)][m-1];
}

unsigned char Date::days_in_month() const {
	return days_in_month(yearval, monthval);
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
	EASSERT(dayval <= 7);
	// days are counted from 0
	return day_names[day_of_week()-1];
}

const char* Date::month_name(void) {
	EASSERT(monthval <= 12);
	// months are counted from 1
	return month_names[monthval-1];
}

Time::Time() : hourval(0), minval(0), secval(0), msecval(0) {}

Time::~Time() {}

bool Time::is_valid(unsigned char h, unsigned char m, unsigned char s, unsigned short ms) {
	if (h <= 23 && m <= 59 && s <= 59 && ms <= 999)
		return true;
	return false;
}

Time::Time(const Time& t) {
	if (!is_valid(t.hour(), t.min(), t.sec(), t.msec()))
		return;
	hourval = t.hour();
	minval = t.min();
	secval = t.sec();
	msecval = t.msec();
}

Time& Time::operator=(const Time& t) {
	if (&t == this)
		return *this;
	if (!is_valid(t.hour(), t.min(), t.sec(), t.msec()))
		return *this;

	hourval = t.hour();
	minval = t.min();
	secval = t.sec();
	msecval = t.msec();
	return *this;
}

bool Time::set(unsigned char h, unsigned char m, unsigned char s, unsigned short ms) {
	EASSERT(is_valid(h, m, s, ms));

	hourval = h;
	minval = m;
	secval = s;
	msecval = ms;

	return true;
}

void Time::set_now(void) {
	struct tm tmp;
	// FIXME: allow UTC/Local option
	get_system_time(&tmp, true);
	hourval = tmp.tm_hour;
	minval  = tmp.tm_min;
	// for leap seconds we can have 61; ignore this and set it to 59
	secval  = (tmp.tm_sec > 59 ? 59 : tmp.tm_sec);
	// FIXME: retrieve clock
	msecval = 0;
}

EDELIB_NS_END // EDELIB_NAMESPACE

/*
 * This code is partialy working since I am not able to correctly extract
 * time delta from zone file. On other hand rest is fine.
 *
 * The code is still here for possible future improvements and desired
 * replacement with above environment setting stuff.
 */

#if 0
unsigned long __myntohl(const unsigned char* c) {
	unsigned long ret = (((unsigned long)c[0])<<24) +
						(((unsigned long)c[1])<<16) +
						(((unsigned long)c[2])<<8) +
						((unsigned long)c[3]);
	return ret;
}

unsigned long convert_time(unsigned long curr_time, char* tzfile) {
	int tzisgmtcnt = ntohl(*(int*)(tzfile+20));
	int tzisstdcnt = ntohl(*(int*)(tzfile+24));
	int tzleapcnt = ntohl(*(int*)(tzfile+28));
	int tztimecnt = ntohl(*(int*)(tzfile+32));
	int tztypecnt = ntohl(*(int*)(tzfile+36));
	int tzcharcnt = ntohl(*(int*)(tzfile+40));

	/*
		printf("tzisgmtcnt: %i tzisstdcnt: %i tzleapcnt: %i tztimecnt: %i tztypecnt: %i tzcharcnt: %i\n",
				tzisgmtcnt, tzisstdcnt, tzleapcnt, tztimecnt, tztypecnt, tzcharcnt);
	*/
	// ???
	int daylight = (tztimecnt>0);
	//daylight = (tztimecnt>0);

	// ???
	// daylight saving time
	int isdst;

	// this should be in class
	long int timezon;

	unsigned char* tzname[2];

	// jump headers
	unsigned char* off = (unsigned char*)(tzfile + 20 + 6 * 4);

	for (int i = 0; i < tztimecnt; i++) {
		if ((time_t)__myntohl(off + i * 4) >= curr_time) {
			unsigned char* tz = off;
			off += tztimecnt * 4;
			i = off[i - 1];
			off += tztimecnt;
			tz += tztimecnt * 5 + tzleapcnt * 4 + tztypecnt * 6;
			off += i * 6;

			isdst = off[4];

			tzname[0] = tz + off[5];
			printf("-> %s\n", tz+off[5]);
			timezon = -(__myntohl(off));

			return (curr_time - timezon);
		}
	}
	return curr_time;
}

bool TimeZone::load(void) {
	if (!pathval || !zoneval)
		return false;

	unsigned int len = strlen(pathval) + strlen(zoneval) + 5;
	char* fullpth = new char[len];

	snprintf(fullpth, len, "%s/%s", pathval, zoneval);
	puts(fullpth);

	int fd = open(fullpth, O_RDONLY);
	if (fd < 0) {
		printf("Failed to open %s\n", fullpth);
		delete [] fullpth;
		return false;
	}

	unsigned int filelen = lseek(fd, 0, SEEK_END);

	char* tzfile = (char*)mmap(0, len, PROT_READ, MAP_SHARED, fd, 0);
	if (tzfile == MAP_FAILED) {
		printf("Failed to mmap %s\n", fullpth);
		delete [] fullpth;
		return false;
	}

	if (ntohl(*(int*)tzfile) != 0x545a6966) {
		printf("ntohl failed for %s\n", fullpth);
		delete [] fullpth;
		return false;
	}

	timeval = convert_time(::time(0), tzfile);

	close(fd);
	delete [] fullpth;

	return true;
}

#endif
