#include <errno.h>
lockf(fildes, mode, size)
	long size;
{
	struct {
		short type, whence; long start, end; short pid;
	} x;
	int i = 8;
	extern int errno;

	x.type = 2;
	x.whence = 1;
	x.start = 0;
	x.end = size;
	switch(mode) {
	case 0:
		x.type = 3;
		break;
	case 1:
		i = 9;
		break;
	case 2:
		break;
	case 3:
		if (fcntl(fildes,7,&x) == -1) {
			return -1;
		}
		if (x.type == 3) {
			return 0;
		}
		errno = EACCES;
		return -1;
	default:
		errno = EINVAL;
		return -1;
	}
	if (fcntl(fildes,i,&x) == -1) {
		if (errno = 79) {
			errno = 78;
		}
		return -1;
	}
}
