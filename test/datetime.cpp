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
}

UT_FUNC(DateCaveat, "Test date caveat")
{
	Date d1, d2;
	d1.set(0, 1, 0);
	d2.set(Date::YearNow, Date::Jan, Date::DayNow);
	UT_VERIFY( d1 == d2 );
}
