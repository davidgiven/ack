/* $Source$
 * $State$
 * $Revision$
 */

/*
 * Derived from dos-gettimeofday.c for newlib-ia16 which was written for the
 * gcc-ia16 toolchain.
 *
 * Copyright (c) 2017--2021 TK Chia
 *
 * The authors hereby grant permission to use, copy, modify, distribute,
 * and license this software and its documentation for any purpose, provided
 * that existing copyright notices are retained in all copies and that this
 * notice is included verbatim in any distributions. No written agreement,
 * license, or royalty fee is required for any of the authorized uses.
 * Modifications to this software may be copyrighted by their authors
 * and need not follow the licensing terms described here, provided that
 * the new terms are clearly indicated on the first page of each file where
 * they apply.
 */

#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include "libsys.h"

extern long _timezone;
extern int _daylight;

int gettimeofday(struct timeval *tv, struct timezone *tz)
{
	static volatile bool tzinited = false;
	struct _dosdate dosdt, olddt;
	struct _dostime dostm;
	suseconds_t usec;
	struct tm tm;
	time_t tim;

	if (!tzinited)
	{
		tzset();
		tzinited = true;
	}

	memset(&tm, 0, sizeof tm);

	_sys_getdate(&dosdt);
	do
	{
		uint8_t hour;

		_sys_gettime(&dostm);
		tm.tm_hour = hour = dostm.hour;
		tm.tm_min = dostm.minute;
		tm.tm_sec = dostm.second;
		usec = dostm.hsecond * 10000UL;

		/* If the time is close to midnight, read the date again to
		 * check for midnight crossover.  If crossover happens,
		 * repeat until it stops. */
		if (hour)
			break;

		olddt = dosdt;
		_sys_getdate(&dosdt);
	} while (dosdt.day != olddt.day);

	tm.tm_year = dosdt.year - 1900;
	tm.tm_mon = dosdt.month - 1;
	tm.tm_mday = dosdt.day;
	tm.tm_isdst = -1;

	tim = mktime(&tm);
	if (tim == -1)
		return -1;

	if (tv)
	{
		tv->tv_sec = tim;
		tv->tv_usec = usec;
	}

	if (tz)
	{
		tz->tz_minuteswest = _timezone / 60;
		tz->tz_dsttime = _daylight;
	}

	return 0;
}
