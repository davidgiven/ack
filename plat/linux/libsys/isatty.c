/*
 * XXX - can't #include <sys/ioctl.h> because libcc.ansi and libsys
 * both provide it, and we might find the wrong one.
 */
#define TIOCGETD 0x5424
int ioctl(int fd, unsigned long, ...);

int isatty(int fd)
{
	int line_disc;
	return 0 <= ioctl(fd, TIOCGETD, &line_disc);
}
