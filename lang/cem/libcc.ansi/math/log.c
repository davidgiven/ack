/*
 * (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */
/* $Header$ */

#include	<errno.h>
#include	<math.h>
#include	"localmath.h"


double
log(double x)
{
	/* log(x) = z*P(z*z)/Q(z*z), z = (x-1)/(x+1), x in [1/sqrt(2), sqrt(2)]
	*/
	/*	Hart & Cheney #2707 */

	static double p[5] = {
		 0.7504094990777122217455611007e+02,
		-0.1345669115050430235318253537e+03,
		 0.7413719213248602512779336470e+02,
		-0.1277249755012330819984385000e+02,
		 0.3327108381087686938144000000e+00
	};

	static double q[5] = {
		 0.3752047495388561108727775374e+02,
		-0.7979028073715004879439951583e+02,
		 0.5616126132118257292058560360e+02,
		-0.1450868091858082685362325000e+02,
		 0.1000000000000000000000000000e+01
	};

	double z, zsqr;
	int exponent;

	if (x <= 0) {
		errno = EDOM;
		return 0;
	}

	x = frexp(x, &exponent);
	while (x < M_1_SQRT2) {
		x += x;
		exponent--;
	}
	z = (x-1)/(x+1);
	zsqr = z*z;
	return z * POLYNOM4(zsqr, p) / POLYNOM4(zsqr, q) + exponent * M_LN2;
}
