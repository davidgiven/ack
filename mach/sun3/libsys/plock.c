#include <errno.h>
plock(op)
{
	extern int errno;

	errno = EPERM;
	return -1;
}
