/*
 * misc - data and miscellaneous routines
 */
/* $Header$ */

#include	<time.h>
#include	<stdlib.h>
#include	<string.h>

#ifdef	__BSD4_2
#include	<sys/time.h>
#elif	!defined(__USG)
struct timeb
{
	timez_t	time;
	unsigned short millitm;
	short timezone;
	short dstflag;
};
#endif

#include	"loc_incl.h"

#ifdef	__USG
long	timezone = -1 * 60;
int	daylight = 1;
char	*tzname[] = {"MET", "MDT",};
#endif

long	__timezone = -1 * 60;
static	int __daylight = 1;
char	*__tzname[] = {"MET", "MDT",};

const char *_days[] = {
		    "Sunday", "Monday", "Tuesday", "Wednesday",
		    "Thursday", "Friday", "Saturday"
	    };

const char *_months[] = {
		    "January", "February", "March",
		    "April", "May", "June",
		    "July", "August", "September",
		    "October", "November", "December"
	    };

const int _ytab[2][12] = {
		{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
		{ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
	};

int
_leap(int year)
{
	return (!(year % 4) && ((year % 100) || !(year % 400)));
}

long
_tzone(void)
{
	register char *p = getenv("TZ");
	register int n = 0;
	int sign = 1;

#ifdef	__BSD4_2
	struct timeval tv;
	struct timezone tz;

	gettimeofday(&tv, &tz);
	__daylight = tz.tz_dsttime;
	__timezone = tz.tz_minuteswest * 60;

#elif !defined(__USG)
	struct timeb time;

	ftime(&time);
	__timezone = time.timezone * 60L;
	__daylight = time.dstflag;
#endif
	if (p && *p) {
		strncpy(__tzname[0], p, 3);
		p += 3;
		if (*p == '-') {
			sign = -1;
			p++;
		}

		while (*p >= '0' && *p <= '9')
			n = 10 * n + (*p++ - '0');
		n *= sign;
		__timezone = ((long)(n * 60)) * 60;
		__daylight = (*p != '\0');
		strncpy(__tzname[1], p, 3);
	}
#ifdef	__USG
	timezone = __timezone;
	daylight = __daylight;
	tzname[0] = __tzname[0];
	tzname[1] = __tzname[1];
#endif
	return __timezone;
}

static int
last_sunday(register int day, register struct tm *timep)
{
	int first = FIRSTSUNDAY(timep);

	if (day >= 58 && LEAPYEAR(1900 + timep->tm_year)) day++;
	if (day < first) return first;
	return day - (day - first) % 7;
}

unsigned
_dstget(struct tm *timep)
{
	int begindst, enddst;

	if(__daylight == -1)
		_tzone();

	timep->tm_isdst = __daylight;
	if (!__daylight) return 0;
	begindst = last_sunday(89, timep);	/* last Sun before Apr */
	enddst = last_sunday(272, timep);	/* last Sun in Sep */

	/* The rules for daylight saving time differ for different coutries.
	 * Implemented here are heuristics that got it right in Holland for
	 * the last couple of years. There is, of course, no universal
	 * algorithm. Only tables would work perfect.
	 */
	if ( (timep->tm_yday>begindst
		|| (timep->tm_yday == begindst && timep->tm_hour >= 2))
	    && (timep->tm_yday<enddst
		|| (timep->tm_yday == enddst && timep->tm_hour < 3))) {
		/* it all happens between 2 and 3 */

		return 60*60;
	}
	timep->tm_isdst = 0;

	return 0;
}
