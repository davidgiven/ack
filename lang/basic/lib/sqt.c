/*
 * (c) copyright 1983 by the Vrije Universiteit, Amsterdam, The Netherlands.
 *
 *          This product is part of the Amsterdam Compiler Kit.
 *
 * Permission to use, sell, duplicate or disclose this software must be
 * obtained in writing. Requests for such permissions may be sent to
 *
 *      Dr. Andrew S. Tanenbaum
 *      Wiskundig Seminarium
 *      Vrije Universiteit
 *      Postbox 7161
 *      1007 MC Amsterdam
 *      The Netherlands
 *
 */

/* $Header $ */

/* Author: J.W. Stevenson */

extern	double	_fef();

/*
	sqrt returns the square root of its floating
	point argument. Newton's method.

	calls _fef
*/

double
_sqt(arg)
double arg;
{
	double x, temp;
	int exp;
	int i;

	if(arg <= 0) {
		if(arg < 0)
			error(3);
		return(0);
	}
	x = _fef(arg,&exp);
	/*
	while(x < 0.5) {
		x =* 2;
		exp--;
	}
	*/
	/*
	 * NOTE
	 * this wont work on 1's comp
	 */
	if(exp & 1) {
		x *= 2;
		exp--;
	}
	temp = 0.5*(1 + x);

	while(exp > 28) {
		temp *= (1<<14);
		exp -= 28;
	}
	while(exp < -28) {
		temp /= (1<<14);
		exp += 28;
	}
	if(exp >= 0)
		temp *= 1 << (exp/2);
	else
		temp /= 1 << (-exp/2);
	for(i=0; i<=4; i++)
		temp = 0.5*(temp + arg/temp);
	return(temp);
}
