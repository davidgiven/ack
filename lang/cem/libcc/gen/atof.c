/* $Id$ */
#ifndef NOFLOAT

extern double strtod();

double
atof(p)
	register char *p;
{
	return strtod(p, (char **) 0);
}
#endif
