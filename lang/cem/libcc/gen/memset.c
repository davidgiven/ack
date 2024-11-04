/* $Id$ */
char *
memset(s, c, n)
	char *s;
	int n;
{
	char *s1 = s;

	while (n--) {
		*s1++ = c;
	}
	return(s);
}
