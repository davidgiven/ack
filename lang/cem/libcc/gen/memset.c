/* $Id$ */
char *
memset(s, c, n)
	char *s;
	register int n;
{
	register char *s1 = s;

	while (n--) {
		*s1++ = c;
	}
	return(s);
}
