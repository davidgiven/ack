/* $Id$ */
#define ALIGN(sz)	((((sz) + (sizeof(long) - 1)) / sizeof(long)) * sizeof(long))
char *
calloc(nelem, elsize)
	unsigned int nelem, elsize;
{
	register char *p;
	register long *q;
	unsigned int size = ALIGN(nelem * elsize);
	extern char *malloc();

	p = malloc(size);
	if (p == 0) return 0;
	q = (long *) (p + size);
	while ((char *) q > p) *--q = 0;
	return p;
}
