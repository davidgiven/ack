/* $Id$ */
ffs(i)
	register int i;
{
	register int n;

	for (n = 8*sizeof(int); n > 0; n--, i >>= 1)
		if ((i&1))
			return (8*sizeof(int) + 1) - n;
	return -1;
}
