#include <errno.h>

ulimit(cmd, newlimit)
	long newlimit;
{
	extern int errno;
	struct {
		long soft, hard;
	} x;
		
	switch(cmd) {
	case 1:
		if (getrlimit(1, &x) < 0) return -1;
		return ((x.soft + 511) & ~511) >> 9;
	case 2:
		x.soft = x.hard = (newlimit << 9);
		if (setrlimit(1, &x) < 0) return -1;
		return x.soft;
	case 3:
		if (getrlimit(2, &x) < 0) return -1;
		return x.soft;
	default:
		errno = EINVAL;
		return -1;
	}
}
