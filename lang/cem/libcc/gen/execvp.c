/* $Id$ */
char    *getenv();
char	*index();

execlp(name, argv)
char *name, *argv;
{
	return(execvp(name, &argv));
}

execvp(name, argv)
char *name, **argv;
{
	char *path = getenv("PATH");
	register char *c = "";
	char progname[1024];

	if (path == 0) path = ":/bin:/usr/bin";
	if (! index(name, '/')) c = path;

	do {
		register char *p = progname;
		register char *n = name;
		char *c1 = c;

		while (*c && *c != ':') {
			*p++ = *c++;
		}
		if (c != c1) *p++ = '/';
		if (*c) c++;
		while (*n) *p++ = *n++;
		*p = 0;

		execv(progname, argv);
	} while (*c);
	return(-1);
}
