#include "bc_string.h"

/* $Id$ */

_intswap(i1,i2)
int *i1,*i2;
{
	int i3;
	i3= *i1;
	*i1= *i2;
	*i2=i3;
}

_fltswap(i1,i2)
double *i1,*i2;
{
	double i3;
	i3= *i1;
	*i1= *i2;
	*i2=i3;
}

_strswap(s1,s2)
String **s1,**s2;
{
	String *s;
	s= *s1;
	*s1= *s2;
	*s2 = s;
}
