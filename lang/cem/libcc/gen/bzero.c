/* $Id$ */
bzero(b, l)
	char *b;
{
	while (l-- > 0) *b++ = 0;
}
