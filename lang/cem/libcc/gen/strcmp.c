/* $Header$ */
int strcmp(s1, s2)
register char *s1, *s2;
{
/* Compare 2 strings. */

  for(;;) {
	if (*s1 != *s2) {
		if (!*s1) return -1;
		if (!*s2) return 1;
		return(*s1 - *s2);
	}
	if (*s1++ == 0) return(0);
	s2++;
  }
}
