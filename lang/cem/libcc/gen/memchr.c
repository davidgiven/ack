/* $Id$ */
char *
memchr(s, c, n)
char *s;
register int n;
{
	register unsigned char *s1 = (unsigned char *) s;
	
	c &= 0377;
	while (n-- > 0) {
		if (*s1 == c) return (char *) s1;
		s1++;
	}
	return 0;
}
