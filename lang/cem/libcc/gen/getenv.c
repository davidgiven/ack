char *getenv(name)
register char *name;
{
  extern char **environ;
  register char **v = environ, *p, *q;

  while ((p = *v++) != 0) {
	q = name;
	while (*q && *q++ == *p++) /* nothing */ ;
	if (*q || *p != '=') continue;
	return(p+1);
  }
  return(0);
}
