#include "object.h"

long
rd_long(fd)
{
	char buf[4];

	rd_bytes(fd, buf, 4L);
	return get4(buf);
}
