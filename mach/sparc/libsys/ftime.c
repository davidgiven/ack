#include <sys/types.h>

ftime(p)
	struct { time_t time; unsigned short millitm;
		 short timezone; short dstflag; } *p;
{
	struct { long l1,l2; } t1,t2;

	if (gettimeofday(&t1,&t2) < 0) return -1;
	p->time = t1.l1;
	p->millitm = t1.l2/1000;
	p->dstflag = t2.l2;
	p->timezone = t2.l1;
	return 0;
}
