#include <sys/types.h>

utime(file, timep)
	char *file;
	time_t timep[2];
{
	struct { long l1,l2,l3,l4; } x;

	x.l2 = x.l4 = 0;
	x.l1 = timep[0];
	x.l3 = timep[1];
	return utimes(file,&x);
}
