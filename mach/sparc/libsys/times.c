#include <sys/types.h>

#define Xval(xx)	((xx).l1*60+(xx).l2/(100000/6))

times(bp)
	struct { time_t l1,l2,l3,l4;} *bp;
{
	struct { struct { long l1,l2; }s1,s2; long x[20]; } t;
	if (getrusage(0,&t) < 0) return -1;
	bp->l1 = Xval(t.s1);
	bp->l2 = Xval(t.s2);
	if (getrusage(-1,&t) < 0) return -1;
	bp->l3 = Xval(t.s1);
	bp->l4 = Xval(t.s2);
	return 0;
}
