/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/*
  Module:	Dummy priority routines
  Author:	Ceriel J.H. Jacobs
  Version:	$Id$
*/
#include "libm2.h"

static unsigned prio = 0;

int stackprio(unsigned int n)
{
	unsigned old = prio;

	if (n > prio)
		prio = n;
	return old;
}

void unstackprio(unsigned int n)
{
	prio = n;
}
