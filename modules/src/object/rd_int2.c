#include "object.h"

int
rd_int2(fd)
{
	char buf[2];

	rd_bytes(fd, buf, 2L);
	return get2(buf);
}
