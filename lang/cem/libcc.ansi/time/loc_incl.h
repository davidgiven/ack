/*
 * loc_incl.h - some local definitions
 */
/* $Header$ */

#define	YEAR1		1900			/* the first year */
#define	SECS_DAY	(24L * 60L * 60L)
#define	LEAPYEAR(year)	(!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define	YEARSIZE(year)	(LEAPYEAR(year) ? 366 : 365)
#define	FIRSTSUNDAY(year)	(((year)->tm_yday - (year)->tm_wday + 420) % 7)
#define	TIME_MAX	ULONG_MAX
#define	ABB_LEN		3

extern const int _ytab[2][12];
extern const char *_days[];
extern const char *_months[];
extern char *__tzname[];

long int _tzone(void);
unsigned _dstget(struct tm *timep);
