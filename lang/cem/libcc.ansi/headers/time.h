/*
 * time.h - date and time
 */
/* $Header$ */

#if	!defined(_TIME_H)
#define	_TIME_H

#define	NULL		((void *)0)

#if	defined(__BSD4_2)
#define	CLK_TCK		1000000		/* ticks per second */
#else
#define	CLK_TCK		60
#endif	/* __BSD4_2 */

#if	!defined(_SIZE_T)
#define	_SIZE_T
typedef unsigned int	size_t;		/* type returned by sizeof */
#endif	/* _SIZE_T */

#if	!defined(_TIME_T)
#define	_TIME_T
typedef	unsigned long	time_t;		/* type returned by TOD clock */
#endif	/* _TIME_T */

typedef	unsigned long	clock_t;	/* type returned by real time clock */

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

clock_t		clock(void);
double		difftime(time_t _time1, time_t _time0);
time_t		mktime(struct tm *_timeptr);
time_t		time(time_t *_timeptr);
char		*asctime(const struct tm *_timeptr);
char		*ctime(const time_t *_timer);
struct	tm	*gmtime(const time_t *_timer);
struct	tm	*localtime(const time_t *_timer);
size_t		strftime(char *_s, size_t _maxsize,
			const char *_format,
			const struct tm *_timeptr);

#if	defined(__USG) || defined(_POSIX_SOURCE)

void	tzset(void);

#if	defined(__USG)
extern long timezone;
extern int daylight;
extern char *tzname[2];
#endif
#endif	/* __USG || _POSIX_SOURCE */

#endif	/* _TIME_H */
