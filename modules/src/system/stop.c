/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#include <system.h>

void
sys_stop(how)
	int how;
{
	switch(how) {
	case S_END:
		exit(0);
	case S_EXIT:
		exit(1);
	case S_ABORT:
	default:
		abort();
	}
}
