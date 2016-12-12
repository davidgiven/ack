#include "bc_string.h"

/* $Id$ */

void _intswap(int* i1, int* i2)
{
	int i3;
	i3= *i1;
	*i1= *i2;
	*i2=i3;
}

void _fltswap(double* i1, double* i2)
{
	double i3;
	i3= *i1;
	*i1= *i2;
	*i2=i3;
}

void _strswap(String** s1, String** s2)
{
	String *s;
	s= *s1;
	*s1= *s2;
	*s2 = s;
}
