/* $Id$ */
int
strncmp(s, t, n)
	register char *s, *t;
	register int n;
{
	while (n-- > 0) {
		if (*s == *t++) {
			if (*s++ == '\0')
				return 0;
		}
		else
			return *s - *--t;
	}
	return 0;
}
