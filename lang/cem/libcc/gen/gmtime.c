/* $Id$ */
#include <time.h>

static int monthsize[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

#define SECS_DAY (24*60L*60L)
#define LEAPYEAR(year)	(!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define YEARSIZE(year)	(LEAPYEAR(year) ? 366 : 365)

struct tm *
gmtime(clock)
	long *clock;
{
	long cl = *clock;
	long dayclock, dayno;
	static struct tm tm_buf;
	register struct tm *pbuf = &tm_buf;
	register int *months = monthsize;
	int year = 1970;

	dayclock = cl % SECS_DAY;
	dayno = cl / SECS_DAY;

	pbuf->tm_sec = dayclock % 60;
	pbuf->tm_min = (dayclock % 3600) / 60;
	pbuf->tm_hour = dayclock / 3600;
	pbuf->tm_wday = (dayno + 4) % 7; /* day 0 was a thursday */
	while (dayno >= YEARSIZE(year)) {
		dayno -= YEARSIZE(year);
		year++;
	}
	pbuf->tm_year = year - 1900;
	pbuf->tm_yday = dayno;
	pbuf->tm_isdst = 0;
	if (YEARSIZE(year) == 366) monthsize[1] = 29;
	while (dayno - *months >= 0) dayno -= *months++;
	pbuf->tm_mday = dayno + 1;
	pbuf->tm_mon = months - monthsize;
	monthsize[1] = 28;
	return pbuf;
}
