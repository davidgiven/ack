#include "object.h"

wr_int2(fd, i)
{
	char buf[2];

	put2(i, buf);
	wr_bytes(fd, buf, 2L);
}
