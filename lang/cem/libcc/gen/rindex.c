char *rindex(s, c)
register char *s, c;
{
  register char *result;

  result = 0;
  do
	if (*s == c)
		result = s;
  while (*s++ != 0);
  return(result);
}
