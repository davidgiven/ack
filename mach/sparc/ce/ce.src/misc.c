/*
misc.c
*/

#include "mach.h"

int power_of_2(n, ref_exp)
int n, *ref_exp;
{
	int exp;

	exp= 0;

	if (n <= 0)
		return 0;

	while (!(n % 2))
	{
		n= n/2;
		exp++;
	}
	if (n != 1)
		return 0;

	if (ref_exp)
		*ref_exp= exp;
	return 1;
}

int uns_power_of_2(n, ref_exp)
unsigned n, *ref_exp;
{
	int exp;

	exp= 0;

	if (!n)
		return 0;

	while (!(n % 2))
	{
		n= n/2;
		exp++;
	}
	if (n != 1)
		return 0;

	if (ref_exp)
		*ref_exp= exp;
	return 1;
}

