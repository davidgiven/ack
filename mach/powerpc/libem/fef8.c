/*
 * $Source$
 * $State$
 * $Revision$
 */

/* no headers allowed! */

/* Given a double, calculates the mantissa and exponent.
 * 
 * This function is intended to be called internally by the code generator,
 * so the calling convention is odd.
 */

int __fef8(double* fp)
{
	double f = *fp;
	int exponent, sign;

	if (f == 0.0)
		return 0;
		
	if (f < 0.0)
	{
		sign = -1;
		f = -f;
	}
	else
		sign = 0;
	
	exponent = 0;
	while (f >= 1.0)
	{
		f /= 2.0;
		exponent++;
	}
	
	while (f < 0.5)
	{
		f *= 2.0;
		exponent--;
	}
	
	*fp = (sign) ? -f : f;
	return exponent;
}
