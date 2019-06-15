/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.

 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include <stdlib.h>
#include <stdio.h>
#include "out.h"
#include "arch.h"
#include "ranlib.h"
#include "object.h"
#include "obj.h"

void rd_ranlib(FILE* fd, register struct ranlib *ran, register long cnt)
{
	rd_bytes(fd, (char *) ran, cnt * SZ_RAN);
	{
		register char *c = (char *) ran + cnt * SZ_RAN;

		ran += cnt;
		while (cnt--) {
			ran--;
			c -= 4; ran->ran_pos = get4(c);
			c -= 4; ran->ran_off = get4(c);
		}
	}
}
