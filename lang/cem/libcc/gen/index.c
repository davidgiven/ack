char *index(s, c)
register char *s, c;
{
  do {
	if (*s == c)
		return(s);
  } while (*s++ != 0);
  return(0);
}
