/* $Header$ */
#ifdef __BSD4_2
#include <sys/time.h>
#else
#include <time.h>
#endif

extern struct tm *localtime();
extern char *asctime();

char *
ctime(clock)
	long *clock;
{
	return asctime(localtime(clock));
}
