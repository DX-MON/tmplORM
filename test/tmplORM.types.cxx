#include <chrono>
#include <crunch++.h>
#include <tmplORM.hxx>
#include "testTypes.hxx"

using systemClock_t = std::chrono::system_clock;

namespace dateTime
{
	using namespace tmplORM::types::baseTypes;
	using namespace tmplORM::types::dateTimeTypes;

	void testCtor(testsuit &suite)
	{
		const ormDateTime_t a;
		suite.assertEqual(a.year(), 0);
		suite.assertEqual(a.month(), 0);
		suite.assertEqual(a.day(), 0);
		suite.assertEqual(a.hour(), 0);
		suite.assertEqual(a.minute(), 0);
		suite.assertEqual(a.second(), 0);
		suite.assertEqual(a.nanoSecond(), 0);

		const ormDateTime_t b{2018, 07, 04, 12, 34, 45, 678901234};
		suite.assertEqual(b.year(), 2018);
		suite.assertEqual(b.month(), 07);
		suite.assertEqual(b.day(), 04);
		suite.assertEqual(b.hour(), 12);
		suite.assertEqual(b.minute(), 34);
		suite.assertEqual(b.second(), 45);
		suite.assertEqual(b.nanoSecond(), 678901234);
	}

	void testFromString(testsuit &suite)
	{
		const ormDateTime_t a{"2018-07-04 12:34:45.678901234"};
		suite.assertEqual(a.year(), 2018);
		suite.assertEqual(a.month(), 07);
		suite.assertEqual(a.day(), 04);
		suite.assertEqual(a.hour(), 12);
		suite.assertEqual(a.minute(), 34);
		suite.assertEqual(a.second(), 45);
		suite.assertEqual(a.nanoSecond(), 678901234);
		const ormDateTime_t b{"2018-07-04 12:34:45.6789"};
		suite.assertEqual(b.year(), 2018);
		suite.assertEqual(b.month(), 07);
		suite.assertEqual(b.day(), 04);
		suite.assertEqual(b.hour(), 12);
		suite.assertEqual(b.minute(), 34);
		suite.assertEqual(b.second(), 45);
		suite.assertEqual(b.nanoSecond(), 678900000);
		const ormDateTime_t c{"2018-07-04 12:34:45.6789012345678"};
		suite.assertEqual(c.year(), 2018);
		suite.assertEqual(c.month(), 07);
		suite.assertEqual(c.day(), 04);
		suite.assertEqual(c.hour(), 12);
		suite.assertEqual(c.minute(), 34);
		suite.assertEqual(c.second(), 45);
		suite.assertEqual(c.nanoSecond(), 678901234);
	}

	void testFromSystemTime(testsuit &suite)
	{
		using seconds = std::chrono::seconds;
		const auto now = systemClock_t::now();
		const ormDateTime_t a{now};

		const time_t time = systemClock_t::to_time_t(now);
		const tm local = *gmtime(&time);

		suite.assertEqual(a.year(), local.tm_year + 1900);
		suite.assertEqual(a.month(), local.tm_mon + 1);
		suite.assertEqual(a.day(), local.tm_mday);
		suite.assertEqual(a.hour(), local.tm_hour);
		suite.assertEqual(a.minute(), local.tm_min);
		suite.assertEqual(a.second(), local.tm_sec);
		suite.assertEqual(a.nanoSecond(), (now.time_since_epoch() - seconds{time}).count());
	}

	void testWrapper(testsuit &suite)
	{
		const _dateTime_t a;
		suite.assertEqual(a.year(), 0);
		suite.assertEqual(a.month(), 0);
		suite.assertEqual(a.day(), 0);
		suite.assertEqual(a.time().count(), 0);
		const _dateTime_t b{2018, 07, 04};
		suite.assertEqual(b.year(), 2018);
		suite.assertEqual(b.month(), 07);
		suite.assertEqual(b.day(), 04);
		suite.assertEqual(b.time().count(), 0);
		const _dateTime_t c{2018, 07, 04, nanoseconds{1234567890}};
		suite.assertEqual(c.year(), 2018);
		suite.assertEqual(c.month(), 07);
		suite.assertEqual(c.day(), 04);
		suite.assertEqual(c.time().count(), 1234567890);
	}
}

namespace types
{
	using irqus::typestring;
	using dateTime_t = tmplORM::types::dateTime_t<typestring<>>;
	using date_t = tmplORM::types::date_t<typestring<>>;
	using namespace tmplORM::types::baseTypes;

	const auto now = ormDateTime_t{systemClock_t::now()};

	void testDateTime(testsuit &suite)
	{
		dateTime_t dateTime;
		const ormDateTime_t a = dateTime.value();
		suite.assertEqual(a.year(), 0);
		suite.assertEqual(a.month(), 0);
		suite.assertEqual(a.day(), 0);
		suite.assertEqual(a.hour(), 0);
		suite.assertEqual(a.minute(), 0);
		suite.assertEqual(a.second(), 0);
		suite.assertEqual(a.nanoSecond(), 0);
		dateTime.value(now);
		suite.assertTrue(dateTime.value() == now);
	}

	void testDate(testsuit &suite)
	{
	}
}
