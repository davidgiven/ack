/* $Id$ */
long atol(s)
register char *s;
{
  register long total = 0;
  register unsigned digit;
  int minus = 0;

  while (*s == ' ' || *s == '\t') s++;
  if (*s == '+') s++;
  else if (*s == '-') {
	s++;
	minus = 1;
  }
  while ((digit = *s++ - '0') < 10) {
	total *= 10;
	total += digit;
  }
  return(minus ? -total : total);
}
