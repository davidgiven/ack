/* $Id$ */
static int bs;
static char *bp;
static char *bufp;
static int sc;
static int bufs;

monitor(lowpc, highpc, buffer, bufsize, nfunc)
	int (*lowpc)(), (*highpc)();
	char *buffer;
{
	long scale;

	if (lowpc == 0) {
		int fd;

		profil((char *) 0, 0, 0, 0);
		if ((fd = creat("mon.out", 0666)) < 0 || !bp) return;
		write(fd, bp, (int) bs);
		close(fd);
		return;
	}

	bs = bufsize << 1;
	bp = buffer;

	*(char **) buffer = (char *) lowpc;
	buffer += sizeof(char *);
	*(char **) buffer = (char *) highpc;
	buffer += sizeof(char *);
	*(int *) buffer = nfunc;
	buffer += sizeof(int);
	buffer += (sizeof (char *) + sizeof(long)) * nfunc;
	bufsize -= ((sizeof (char *) + sizeof(long)) * nfunc + 2 * sizeof(char *) + sizeof(int)) >> 1;
	if (bufsize < 0) return;
	scale = ((char *) highpc - (char *) lowpc) >> 1;
	if (bufsize < scale)
		scale = ((long) bufsize << 15) / scale;
	else	scale = 0x8000;
	bufp = buffer;
	sc = scale << 1;
	bufs = bufsize << 1;
	profil(buffer, bufs, lowpc, sc);
}

moncontrol(mode)
{
	profil(bufp, bufs, *(char **) bp, !mode ? 0 : sc);
}

#define NCOUNTS 300

monstartup(lowpc, highpc)
	int (*lowpc)(), (*highpc)();
{
	int sz = (((char *) highpc - (char *) lowpc + 7) & ~7);
	char *s, *sbrk();

	sz += NCOUNTS * (sizeof(long) + sizeof(char *)) + 2 * sizeof(char *) + sizeof(int);
	s = sbrk(sz);
	if ((int) s == -1) return;
	monitor(lowpc, highpc, s, sz >> 1, NCOUNTS);
}
