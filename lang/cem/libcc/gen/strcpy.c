/* $Id$ */
char *strcpy(s1, s2)
register char *s1, *s2;
{
/* Copy s2 to s1. */
  char *original = s1;

  while (*s1++ = *s2++) /* nothing */;
  return(original);
}
