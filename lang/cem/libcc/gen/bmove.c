/* $Id$ */
bmove(dst, src, len)
	char *dst, *src;
	int len;
{
	bcopy(src, dst, len);
}
