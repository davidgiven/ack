/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.

 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include <stdlib.h>
#include <stdio.h>
#include "out.h"
#include "object.h"
#include "obj.h"

void wr_int4(FILE* fd, long l)
{
	char buf[4];

	put4(l, buf);
	wr_bytes(fd, buf, 4L);
}
