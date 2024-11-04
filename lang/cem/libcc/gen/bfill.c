/* $Id$ */
bfill(dst, len, fill)
	char *dst;
	int len;
	int fill;
{
	while (--len >= 0)
		*dst++ = fill;
}
