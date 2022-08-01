#include <sgtty.h>

int isatty(int fd)
{
	char buf[128];
	/* not a sgttyb struct; it might not be large enough;
	   I know for a fact that it is'nt large enough on PC/IX,
	   where gtty is an ioctl(..., TCGETA, ...)
	*/

	if (gtty(fd, (void*) buf) < 0) return 0;
	return 1;
}
