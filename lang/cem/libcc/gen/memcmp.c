/* $Id$ */
int
memcmp(s1, s2, n)
register char *s1, *s2;
{
/* Compare 2 strings. */

  while (n-- > 0) {
	if (*s1 != *s2) {
		return(*s1 - *s2);
	}
	s1++;
	s2++;
  }
  return 0;
}
