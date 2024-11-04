/* $Id$ */
char *
memccpy(dst, src, c, n)
	char *dst, *src;
	int n;
{
	while (n-- > 0) {
		if ((*dst++ = *src++) == c) return (char *) dst;
	}

	return 0;
}
