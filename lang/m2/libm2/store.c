_store(siz, addr, p)
	register char *addr;
	register int siz;
{
	register char *q = (char *) &p;

	while (siz--) *addr++ = *q++;
}
