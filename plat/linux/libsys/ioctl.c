#include "libsys.h"

/*
 * The usual prototype is ioctl(int, unsigned long, ...).  We use a
 * different prototype to easily get argp, and we don't include any
 * header file that would declare the usual prototype.
 */
int ioctl(int fd, unsigned long request, void *argp)
{
	return _syscall(__NR_ioctl, fd, request, argp);
}
