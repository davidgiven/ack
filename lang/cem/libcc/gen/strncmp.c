/* $Header$ */
int
strncmp(s1, s2, n)
	register char *s1, *s2;
	int n;
{
/* Compare two strings, but at most n characters. */

  while (n-- > 0) {
	if (*s1 != *s2) {
		if (!*s1) return -1;
		if (!*s2) return 1;
		return(*s1 - *s2);
	}
	if (*s1++ == 0) break;
	s2++;
  }
  return 0;
}
