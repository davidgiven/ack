/* $Header$ */
#include <time.h>

#define DATE_STR "??? ??? ?? ??:??:?? ????\n"

static char *days[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

static char *months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
			  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

static char *two_digits();
static char *four_digits();

char *
asctime(tm)
	register struct tm *tm;
{
	static char buf[32];
	register char *pb = buf, *ps;
	
	strcpy(pb, DATE_STR);
	ps = days[tm->tm_wday];
	while (*ps) *pb++ = *ps++;
	pb++;
	ps = months[tm->tm_mon];
	while (*ps) *pb++ = *ps++; 
	pb++;
	pb = two_digits(
		two_digits(
		   two_digits(
		      two_digits(pb, tm->tm_mday, 0),
		      tm->tm_hour, 1),
		   tm->tm_min, 1),
		tm->tm_sec, 1);
	four_digits(pb, tm->tm_year+1900);
	return(buf);
}

static char *
two_digits(pb, i, nospace)
	register char *pb;
{
	*pb = (i / 10) % 10 + '0';
	if (!nospace && *pb == '0') *pb = ' ';
	pb++;
	*pb++ = (i % 10) + '0';
	return ++pb;
}

static char *
four_digits(pb, i)
	register char *pb;
{
	i %= 10000;
	*pb++ = (i / 1000) + '0';
	i %= 1000;
	*pb++ = (i / 100) + '0';
	i %= 100;
	*pb++ = (i / 10) + '0';
	*pb++ = (i % 10) + '0';
	return ++pb;
}
