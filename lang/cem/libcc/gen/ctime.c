/* $Header$ */
#include <time.h>

extern struct tm *localtime();
extern char *asctime();

char *
ctime(clock)
	long *clock;
{
	return asctime(localtime(clock));
}
