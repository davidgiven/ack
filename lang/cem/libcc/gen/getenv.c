/* $Id$ */
char *getenv(name)
register char *name;
{
  extern char **environ;
  register char **v = environ, *p, *q;

  if (v == 0 || name == 0) return 0;
  while ((p = *v++) != 0) {
	q = name;
	while (*q && *q++ == *p++) /* nothing */ ;
	if (*q || *p != '=') continue;
	return(p+1);
  }
  return(0);
}
