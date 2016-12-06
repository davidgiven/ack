#include <sys/ioctl.h>

int isatty(int fd)
{
	int line_disc;
	return 0 <= ioctl(fd, TIOCGETD, &line_disc);
}
