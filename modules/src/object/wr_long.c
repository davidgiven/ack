#include "object.h"

wr_long(fd, l)
	long l;
{
	char buf[4];

	put4(l, buf);
	wr_bytes(fd, buf, 4L);
}
