#include "object.h"

unsigned int
rd_unsigned2(fd)
{
	char buf[2];

	rd_bytes(fd, buf, 2L);
	return uget2(buf);
}
