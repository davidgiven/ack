/*
 * (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* $Header$ */

#include <math.h>
#include <errno.h>

extern int errno;

double
tan(x)
	double x;
{
	/*	First reduce range to [0, pi/4].
		Then use approximation tan(x*pi/4) = x * P(x*x)/Q(x*x).
		Hart & Cheney # 4288
		Use: tan(x) = 1/tan(pi/2 - x) 
		     tan(-x) = -tan(x)
		     tan(x+k*pi) = tan(x)
	*/

	static double p[5] = {
		-0.5712939549476836914932149599e+10,
		 0.4946855977542506692946040594e+09,
		-0.9429037070546336747758930844e+07,
		 0.5282725819868891894772108334e+05,
		-0.6983913274721550913090621370e+02
	};

	static double q[6] = {
		-0.7273940551075393257142652672e+10,
		 0.2125497341858248436051062591e+10,
		-0.8000791217568674135274814656e+08,
		 0.8232855955751828560307269007e+06,
		-0.2396576810261093558391373322e+04,
		 0.1000000000000000000000000000e+01
	};

	int negative = x < 0;
	double tmp, tmp1, tmp2;
	double xsq;
	int invert = 0;
	int ip;

	if (negative) x = -x;

	/*	first reduce to [0, pi)	*/
	if (x >= M_PI) {
	    if (x <= 0x7fffffff) {
		/*	Use extended precision to calculate reduced argument.
			Split pi in 2 parts a1 and a2, of which the first only
			uses some bits of the mantissa, so that n * a1 is
			exactly representable, where n is the integer part of
			x/pi.
			Here we used 12 bits of the mantissa for a1.
			Also split x in integer part x1 and fraction part x2.
			We then compute x-n*pi as ((x1 - n*a1) + x2) - n*a2.
		*/
#define A1 3.14111328125
#define A2 0.00047937233979323846264338327950288
		double n = (long) (x / M_PI);
		double x1 = (long) x;
		double x2 = x - x1;
		x = x1 - n * A1;
		x += x2;
		x -= n * A2;
#undef A1
#undef A2
	    }
	    else {
		extern double modf();

		x = modf(x/M_PI, &tmp) * M_PI;
	    }
	}
	/*	because the approximation uses x*pi/4, we reverse this */
	x /= M_PI_4;
	ip = (int) x;
	x -= ip;

	switch(ip) {
	case 0:
		/* [0,pi/4] */
		break;
	case 1:
		/* [pi/4, pi/2]
		   tan(x+pi/4) = 1/tan(pi/2 - (x+pi/4)) = 1/tan(pi/4 - x)
		*/
		invert = 1;
		x = 1.0 - x;
		break;
	case 2:
		/* [pi/2, 3pi/4]
		   tan(x+pi/2) = tan((x+pi/2)-pi) = -tan(pi/2 - x) =
		   -1/tan(x)
		*/
		negative = ! negative;
		invert = 1;
		break;
	case 3:
		/* [3pi/4, pi)
		   tan(x+3pi/4) = tan(x-pi/4) = - tan(pi/4-x)
		*/
		x = 1.0 - x;
		negative = ! negative;
		break;
	}
	xsq = x * x;
	tmp1 = x*POLYNOM4(xsq, p);
	tmp2 = POLYNOM5(xsq, q);
	tmp = tmp1 / tmp2;
	if (invert) {
		if (tmp == 0.0) {
			errno = ERANGE;
			tmp = HUGE;
		}
		else tmp = tmp2 / tmp1;
	}

	return negative ? -tmp : tmp;
}
