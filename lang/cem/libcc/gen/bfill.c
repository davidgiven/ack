/* $Id$ */
bfill(dst, len, fill)
	register char *dst;
	register int len;
	register int fill;
{
	while (--len >= 0)
		*dst++ = fill;
}
