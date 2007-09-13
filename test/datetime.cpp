#include <edelib/DateTime.h>
#include "UnitTest.h"

#include <stdio.h>

using namespace edelib;

UT_FUNC(DateTest, "Test date")
{
	Date d;

	d.set(2004, 10, 20);

	UT_VERIFY( d.day() == 20 );
	UT_VERIFY( d.month() == 10 );
	UT_VERIFY( d.year() == 2004 );
	UT_VERIFY( d.days_in_month() == 31 );

	// was wednsday, we are counting from Sunday
	UT_VERIFY( d.day_of_week() == 4 );

	// 2004 was leap year
	UT_VERIFY( d.leap_year() == true );

	d.set(2007, 1, 1);
	UT_VERIFY( d.days_in_month() == 31 );
	UT_VERIFY( d.leap_year() == false );
	d.set(2007, 2, 1);
	UT_VERIFY( d.days_in_month() == 28 );
	// thursday
	UT_VERIFY( d.day_of_week() == 5 );

	d.set(2007, 2, 24);
	// sunday
	UT_VERIFY( d.day_of_week() == 7 );

	d.set(2004, 10, 20);

	// assignement
	Date d2 = d;
	UT_VERIFY( d2.day() == 20 );
	UT_VERIFY( d2.month() == 10 );
	UT_VERIFY( d2.year() == 2004 );
	UT_VERIFY( d2.days_in_month() == 31 );
	// was wednsday, we are counting from Sunday
	UT_VERIFY( d2.day_of_week() == 4 );
	// 2004 was leap year
	UT_VERIFY( d2.leap_year() == true );

	UT_VERIFY( d == d2 );
	UT_VERIFY( d >= d2 );
	UT_VERIFY( d <= d2 );

	d.set(2005, 1, 1);
	UT_VERIFY( d.day_of_year() == 1 );
	d.set(2005, 1, 31);
	UT_VERIFY( d.day_of_year() == 31 );
	d.set(2007, 5, 6);
	UT_VERIFY( d.day_of_year() == 126 );
	d.set(2007, 5, 16);
	UT_VERIFY( d.day_of_year() == 136 );
	d.set(2007, 12, 31);
	UT_VERIFY( d.day_of_year() == 365 );

	// leap year
	d.set(2004, 12, 31);
	UT_VERIFY( d.day_of_year() == 366 );
}

UT_FUNC(DateOperatorsTest, "Test date operators")
{
	Date d1;
	d1.set(Date::YearNow, Date::MonthNow, Date::DayNow);

	Date d2(d1);
	UT_VERIFY( d1 == d2 );
	UT_VERIFY( d1 >= d2 );
	UT_VERIFY( d1 <= d2 );
	UT_VERIFY( d1.day() == d2.day() );
	UT_VERIFY( d1.month() == d2.month() );
	UT_VERIFY( d1.year() == d2.year() );

	d1.set(Date::YearNow, 2, 15);
	d2.set(Date::YearNow, 3, 1);

	UT_VERIFY( d1 < d2 );
	UT_VERIFY( d1 <= d2 );
	UT_VERIFY( d1 != d2 );

	UT_VERIFY( d1.year() == d2.year() );
	UT_VERIFY( d1.month() < d2.month() );
	UT_VERIFY( d1.day() > d2.day() );

	d1.set(2004, Date::Jan, Date::DayNow);
	d2.set(2005, Date::Jan, Date::DayNow);

	UT_VERIFY( d2 > d1 );
	UT_VERIFY( d2.month() == d1.month() );
	UT_VERIFY( d2.day() == d1.day() );
	UT_VERIFY( d2.days_in_month() == d1.days_in_month() );

	d1.set(2004, 5, 5);
	d2.set(2005, 5, 5);

	UT_VERIFY( d2 > d1 );
	UT_VERIFY( d2 != d1 );

	++d1;
	UT_VERIFY( d1.year() == 2004 );
	UT_VERIFY( d1.month() == 5 );
	UT_VERIFY( d1.day() == 6 );

	d1.set(2004, 1, 30);
	++d1;
	UT_VERIFY( d1.year() == 2004 );
	UT_VERIFY( d1.month() == 1 );
	UT_VERIFY( d1.day() == 31 );

	d1.set(2004, 1, 31);
	++d1;
	UT_VERIFY( d1.year() == 2004 );
	UT_VERIFY( d1.month() == 2 );
	UT_VERIFY( d1.day() == 1 );

	// 2004 was leap year
	d1.set(2004, 2, 29);
	++d1;
	UT_VERIFY( d1.year() == 2004 );
	UT_VERIFY( d1.month() == 3 );
	UT_VERIFY( d1.day() == 1 );

	d1.set(2004, 12, 31);
	++d1;
	UT_VERIFY( d1.year() == 2005 );
	UT_VERIFY( d1.month() == 1 );
	UT_VERIFY( d1.day() == 1 );

	// decrements
	d1.set(2004, 5, 5);
	--d1;
	UT_VERIFY( d1.year() == 2004 );
	UT_VERIFY( d1.month() == 5 );
	UT_VERIFY( d1.day() == 4 );

	d1.set(2005, 1, 1);
	--d1;
	UT_VERIFY( d1.year() == 2004 );
	UT_VERIFY( d1.month() == 12 );
	UT_VERIFY( d1.day() == 31 );

	// 2004 was leap year
	d1.set(2004, 3, 1);
	--d1;
	UT_VERIFY( d1.year() == 2004 );
	UT_VERIFY( d1.month() == 2 );
	UT_VERIFY( d1.day() == 29 );

	d1.set(2004, 5, 5);
	d2 = d1;
	--d1;
	++d1;
	UT_VERIFY( d1 == d2 );
}

UT_FUNC(DateCaveat, "Test date caveat")
{
	Date d1, d2;
	d1.set(0, 1, 0);
	d2.set(Date::YearNow, Date::Jan, Date::DayNow);
	UT_VERIFY( d1 == d2 );
}

UT_FUNC(TimeTest, "Test time")
{
	Time t;
	t.set(20, 10, 10, 233);
	UT_VERIFY( t.hour() == 20 );
	UT_VERIFY( t.min() == 10 );
	UT_VERIFY( t.sec() == 10 );
	UT_VERIFY( t.msec() == 233 );

	UT_VERIFY( Time::is_valid(25, 0, 0, 0) == false );
	UT_VERIFY( Time::is_valid(0, 0, 0, 0) == true );
}

UT_FUNC(TimeOperatorsTest, "Test time operators")
{
	Time t1, t2;
	UT_VERIFY( t1 == t2 );
	t1.set(0, 0, 0, 1);
	UT_VERIFY( t1 > t2);

	t1.set(0, 59, 59, 59);
	t2.set(1, 0, 0, 0);
	UT_VERIFY( t1 > t2 );
	UT_VERIFY( t1 >= t2 );
	UT_VERIFY( t1 != t2 );

	t2 = t1;
	UT_VERIFY( t1 == t2 );
	UT_VERIFY( t1 >= t2 );
	UT_VERIFY( t1 <= t2 );

	Time t3(t1);
	UT_VERIFY( t1 == t3 );
	UT_VERIFY( t1 >= t3 );
	UT_VERIFY( t1 <= t3 );
}
