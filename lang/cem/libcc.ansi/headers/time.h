/*
 * time.h - date and time
 */
/* $Header$ */

#ifndef	_TIME_HEADER_
#define	_TIME_HEADER_

#ifndef	NULL
#define	NULL		0
#endif	/* NULL */

#ifdef	__BSD4_2
#define	CLK_TCK		60		/* ticks per second */
#else
#define	CLK_TCK		1
#endif	/* __BSD4_2 */

#ifndef	_TYPE_SIZE_
#define	_TYPE_SIZE_
typedef unsigned int	size_t;		/* type returned by sizeof */
#endif	/* _TYPE_SIZE_ */

typedef	signed	long	time_t;		/* type returned by TOD clock */
typedef	signed	long	clock_t;	/* type returned by real time clock */

struct tm {
	int	tm_sec;			/* seconds after the minute - [0, 59] */
	int	tm_min;			/* minutes after the hour - [0, 59] */
	int	tm_hour;		/* hours since midnight - [0, 28] */
	int	tm_mday;		/* day of the month - [1, 31] */
	int	tm_mon;			/* months since January - [0, 11] */
	int	tm_year;		/* years since 1900 */
	int	tm_wday;		/* days since Sunday - [0, 6] */
	int	tm_yday;		/* days since January 1 - [0, 365] */
	int	tm_isdst;		/* Daylight Saving Time flag */
};

clock_t		clock(void);
double		difftime(time_t time1, time_t time0);
time_t		mktime(struct tm *timeptr);
time_t		time(time_t *timeptr);
char		*asctime(const struct tm *timeptr);
char		*ctime(const time_t *timer);
struct	tm	*gmtime(const time_t *timer);
struct	tm	*localtime(const time_t *timer);
size_t		strftime(char *s, size_t maxsize,
			const char *format,
			const struct tm *timeptr);

#endif	/* _TIME_HEADER_ */
