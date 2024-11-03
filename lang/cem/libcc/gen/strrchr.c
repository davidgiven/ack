/* $Id$ */
char *strrchr(s, c)
char *s, c;
{
  char *result;

  result = 0;
  do
	if (*s == c)
		result = s;
  while (*s++ != 0);
  return(result);
}
