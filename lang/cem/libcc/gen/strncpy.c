/* $Id$ */
char
*strncpy(s1, s2, n)
register char *s1, *s2;
int n;
{
/* Copy s2 to s1, but at most n characters. */

  char *original = s1;

  while (*s2 && n-- > 0) *s1++ = *s2++;
  while (n-- > 0) *s1++ = '\0';
  return(original);
}
