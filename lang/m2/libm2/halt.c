/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/*
  Module:	program termination routines
  Author:	Ceriel J.H. Jacobs
  Version:	$Header$
*/
#define MAXPROCS 16

static int callindex;
static int (*proclist[MAXPROCS])();

_cleanup()
{
	register int i;

	for (i = 0; i < callindex; i++) {
		(*proclist[i])();
	}
	callindex = 0;
}

CallAtEnd(p)
	int (*p)();
{
	if (callindex >= MAXPROCS) {
		return 0;
	}
	else {
		proclist[callindex++] = p;
	}
	return 1;
}

halt()
{
	exit(0);
}
