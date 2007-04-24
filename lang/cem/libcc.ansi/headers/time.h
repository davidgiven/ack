/*
 * time.h - date and time
 */
/* $Id$ */

#ifndef _TIME_H
#define	_TIME_H

#include <stddef.h>

#define CLOCKS_PER_SEC 1000000

typedef	unsigned long time_t;		/* type returned by TOD clock */
typedef	unsigned long clock_t;	/* type returned by real time clock */

struct tm {
	int	tm_sec;			/* seconds after the minute - [0, 59] */
	int	tm_min;			/* minutes after the hour - [0, 59] */
	int	tm_hour;		/* hours since midnight - [0, 23] */
	int	tm_mday;		/* day of the month - [1, 31] */
	int	tm_mon;			/* months since January - [0, 11] */
	int	tm_year;		/* years since 1900 */
	int	tm_wday;		/* days since Sunday - [0, 6] */
	int	tm_yday;		/* days since January 1 - [0, 365] */
	int	tm_isdst;		/* Daylight Saving Time flag */
};

extern clock_t clock(void);
extern double difftime(time_t _time1, time_t _time0);
extern time_t mktime(struct tm *_timeptr);
extern time_t time(time_t *_timeptr);
extern char* asctime(const struct tm *_timeptr);
extern char* ctime(const time_t *_timer);
extern struct tm* gmtime(const time_t *_timer);
extern struct tm* localtime(const time_t *_timer);
extern size_t strftime(char *_s, size_t _maxsize,
			const char *_format,
			const struct tm *_timeptr);

/* Extensions not in the standard */

#define ctime(t)  asctime(localtime(t))

#endif
