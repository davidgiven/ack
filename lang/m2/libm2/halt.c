#define MAXPROCS 16

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
		return 0;
	}
	else {
		proclist[callindex++] = p;
	}
	return 1;
}

_halt()
{
	exit(0);
}
