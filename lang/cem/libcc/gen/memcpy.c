/* $Id$ */
char *
memcpy(s1, s2, n)
register char *s1, *s2;
register int n;
{
/* Copy a block of data. */

	char *ret = s1;

	if (s2 <= s1 && s2 + (n-1) >= s1) {
		s1 += n; s2 += n;
		while (n-- > 0) *--s1 = *--s2;
	}
	else	while (n-- > 0) *s1++ = *s2++;
	return ret;
}
