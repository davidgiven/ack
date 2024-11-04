/* $Id$ */
#ifndef NOFLOAT

extern double strtod();

double
atof(p)
	char *p;
{
	return strtod(p, (char **) 0);
}
#endif
