/*
 * mktime - convert local time into calendar time
 */
/* $Header$ */

#include	<time.h>
#include	<limits.h>
#include	"loc_incl.h"

time_t
mktime(register struct tm *timep)
{
	int day, year, month, yday;
	long tmp_sec, tz = _tzone();
	register time_t seconds, localseconds;
	int overflow = 0;
	unsigned dst;


	timep->tm_min += timep->tm_sec / 60;
	timep->tm_sec %= 60;
	if (timep->tm_sec < 0) {
		timep->tm_sec += 60;
		timep->tm_min--;
	}
	timep->tm_hour += timep->tm_min / 60;
	timep->tm_min = timep->tm_min % 60;
	if (timep->tm_min < 0) {
		timep->tm_min += 60;
		timep->tm_hour--;
	}
	day = timep->tm_hour / 24;
	timep->tm_hour= timep->tm_hour % 24;
	if (timep->tm_hour < 0) {
		timep->tm_hour += 24;
		day--;
	}
	timep->tm_year += timep->tm_mon / 12;
	timep->tm_mon %= 12;
	if (timep->tm_mon < 0) {
		timep->tm_mon += 12;
		timep->tm_year--;
	}
	day += (timep->tm_mday - 1);
	while (day < 0) {
		day += YEARSIZE(YEAR1 + timep->tm_year - 1);
		timep->tm_year--;
	}
	while (day >= YEARSIZE(YEAR1 + timep->tm_year)) {
		day -= YEARSIZE(YEAR1 + timep->tm_year);
		timep->tm_year++;
	}
	while (day >= _ytab[LEAPYEAR(YEAR1 + timep->tm_year)][timep->tm_mon]) {
		day -= _ytab[LEAPYEAR(YEAR1 + timep->tm_year)][timep->tm_mon];
		if (++(timep->tm_mon) == 12) {
			timep->tm_mon = 0;
			timep->tm_year++;
		}
	}
	timep->tm_mday = day + 1;
	year = 70;
	if (timep->tm_year < year) return -1;
	seconds = 0;
	while (!overflow && year < timep->tm_year) {
		tmp_sec = SECS_DAY * YEARSIZE(YEAR1 + year);
		if (TIME_MAX - tmp_sec <= seconds) overflow++;
		else {
			seconds += tmp_sec;
			year++;
		}
	}
	yday = month = 0;
	while (!overflow && month < timep->tm_mon) {
		yday += _ytab[LEAPYEAR(YEAR1 + year)][month];
		month++;
	}
	yday += (timep->tm_mday - 1);
	if (!overflow) {
		tmp_sec = yday * SECS_DAY;
		if (TIME_MAX - tmp_sec <= seconds) overflow++;
		else seconds += tmp_sec;
	}

	timep->tm_yday = yday;		/* ??? assignments should be later */
					/* day 0 was thursday (4) */
	timep->tm_wday = (seconds / SECS_DAY + 4) % 7;

	if (timep->tm_isdst < 0)
		dst = _dstget(timep);
	else if (timep->tm_isdst)
		dst = 60 * 60;
	else dst = 0;

	if (!overflow) {
		tmp_sec = timep->tm_hour * 60 * 60;
		if (TIME_MAX - tmp_sec <= seconds) overflow++;
		else seconds += tmp_sec;
	}

	if (!overflow) {
		tmp_sec = timep->tm_min * 60;
		if (TIME_MAX - tmp_sec <= seconds) overflow++;
		else seconds += tmp_sec;
	}

	if (!overflow) {
		tmp_sec = timep->tm_sec;
		if (TIME_MAX - tmp_sec <= seconds) overflow++;
		else seconds += tmp_sec;
	}

	localseconds = seconds;

	if (!overflow) {
		tmp_sec = tz;
		if (((tmp_sec > 0) && (TIME_MAX - tmp_sec <= seconds))
		    || ((tmp_sec < 0) && (seconds < -tmp_sec)))
			overflow++;
		else seconds += tmp_sec;
	}

	if (!overflow) {
		tmp_sec = dst;
		if (tmp_sec > seconds) overflow++;
		else seconds -= tmp_sec;
	}

	if (overflow) return (time_t)-1;

	return seconds;
}
