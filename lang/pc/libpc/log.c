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

/* Author: J.W. Stevenson */

#include	<pc_err.h>

extern double	_fef();
extern		_trp();

/*
	log returns the natural logarithm of its floating
	point argument.

	The coefficients are #2705 from Hart & Cheney. (19.38D)

	It calls _fef.
*/

#define	HUGE	1.701411733192644270e38

static double log2	= 0.693147180559945309e0;
static double sqrto2	= 0.707106781186547524e0;
static double p0	= -.240139179559210510e2;
static double p1	= 0.309572928215376501e2;
static double p2	= -.963769093368686593e1;
static double p3	= 0.421087371217979714e0;
static double q0	= -.120069589779605255e2;
static double q1	= 0.194809660700889731e2;
static double q2	= -.891110902798312337e1;

double
_log(arg)
double arg;
{
	double x,z, zsq, temp;
	int exp;

	if(arg <= 0) {
		_trp(ELOG);
		return(-HUGE);
	}
	x = _fef(arg,&exp);
	/*
	while(x < 0.5) {
		x =* 2;
		exp--;
	}
	*/
	if(x<sqrto2) {
		x *= 2;
		exp--;
	}

	z = (x-1)/(x+1);
	zsq = z*z;

	temp = ((p3*zsq + p2)*zsq + p1)*zsq + p0;
	temp = temp/(((zsq + q2)*zsq + q1)*zsq + q0);
	temp = temp*z + exp*log2;
	return(temp);
}
