/* $Id$ */
bzero(b, l)
	register char *b;
{
	while (l-- > 0) *b++ = 0;
}
