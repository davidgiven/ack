/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#include <stdlib.h>
#include "system.h"

void sys_stop(int how)
{
	switch(how) {
	case S_END:
		exit(EXIT_SUCCESS);
	case S_EXIT:
		exit(EXIT_FAILURE);
	case S_ABORT:
	default:
		abort();
	}
}
