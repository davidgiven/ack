#ifdef BSD4_2
#include <sys/time.h>
#else
#ifndef USG
#include <sys/types.h>
#include <sys/timeb.h>
#endif
#endif

long	timezone = -1 * 60;
int	daylight = 1;
char	*tzname[] = {"MET", "MDT",};

tzset()
{
#ifdef BSD4_2
	struct timeval tval;
	struct timezone tzon;

	gettimeofday(&tval, &tzon);
	timezone = tzon.tz_minuteswest * 60L;
	daylight = tzon.tz_dsttime;
#else
#ifndef USG
	struct timeb time;

	ftime(&time);
	timezone = time.timezone*60L;
	daylight = time.dstflag;
#endif
#endif

	{
	extern char *getenv();
	register char *p = getenv("TZ");

	if (p && *p) {
		register int n = 0;
		int sign = 1;

		strncpy(tzname[0], p, 3);
		p += 3;
		if (*(p += 3) == '-') {
			sign = -1;
			p++;
		}

		while(*p >= '0' && *p <= '9')
			n = 10 * n + (*p++ - '0');
		n *= sign;
		timezone = ((long)(n * 60)) * 60;
		daylight = (*p != '\0');
		strncpy(tzname[1], p, 3);
	}
	}
}
