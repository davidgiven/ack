/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/*
  Module:	program termination routines
  Author:	Ceriel J.H. Jacobs
  Version:	$Id$
*/

#include <unistd.h>
#include "libm2.h"
#define MAXPROCS 32

static int callindex = 0;
static void (*proclist[MAXPROCS])(void);

void _cleanup(void)
{
	while (--callindex >= 0)
		(*proclist[callindex])();
	callindex = 0;
}

int CallAtEnd(void (*p)(void))
{
	if (callindex >= MAXPROCS)
	{
		return 0;
	}
	proclist[callindex++] = p;
	return 1;
}

void halt(void)
{
	_cleanup();
	_exit(0);
}
