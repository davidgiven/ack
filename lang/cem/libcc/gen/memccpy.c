/* $Id$ */
char *
memccpy(dst, src, c, n)
	register char *dst, *src;
	register int n;
{
	while (n-- > 0) {
		if ((*dst++ = *src++) == c) return (char *) dst;
	}

	return 0;
}
