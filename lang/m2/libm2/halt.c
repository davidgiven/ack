#define MAXPROCS 20
#include <m2_traps.h>

static int callindex;
static int (*proclist[MAXPROCS])();

_cleanup()
{
	register int i;

	for (i = 0; i < callindex; i++) {
		(*proclist[i])();
	}
}

CallAtEnd(p)
	int (*p)();
{
	if (callindex >= MAXPROCS) {
		TRP(M2_ENDPROCS);
	}
	else {
		proclist[callindex++] = p;
	}
}

_halt()
{
	exit(0);
}
