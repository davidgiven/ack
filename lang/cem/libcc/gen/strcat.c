/* $Id$ */
char *strcat(s1, s2)
register char *s1, *s2;
{
  /* Append s2 to the end of s1. */

  char *original = s1;

  /* Find the end of s1. */
  while (*s1++ != 0) ;

  s1--;
  /* Now copy s2 to the end of s1. */
  while (*s1++ = *s2++) /* nothing */ ;
  return(original);
}
