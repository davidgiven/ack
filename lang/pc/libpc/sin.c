/*
 * (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* $Header$ */

#include <math.h>

static double
sinus(x, quadrant)
	double x;
{
	/*	sin(0.5*pi*x) = x * P(x*x)/Q(x*x) for x in [0,1] */
	/*	Hart & Cheney # 3374 */

	static double p[6] = {
		 0.4857791909822798473837058825e+10,
		-0.1808816670894030772075877725e+10,
		 0.1724314784722489597789244188e+09,
		-0.6351331748520454245913645971e+07,
		 0.1002087631419532326179108883e+06,
		-0.5830988897678192576148973679e+03
	};

	static double q[6] = {
		 0.3092566379840468199410228418e+10,
		 0.1202384907680254190870913060e+09,
		 0.2321427631602460953669856368e+07,
		 0.2848331644063908832127222835e+05,
		 0.2287602116741682420054505174e+03,
		 0.1000000000000000000000000000e+01
	};

	double xsqr;
	int t;

	if (x < 0) {
		quadrant += 2;
		x = -x;
	}
	if (M_PI_2 - x == M_PI_2) {
		switch(quadrant) {
		case 0:
		case 2:
			return 0.0;
		case 1:
			return 1.0;
		case 3:
			return -1.0;
		}
	}
	if (x >= M_2PI) {
	    if (x <= 0x7fffffff) {
		/*	Use extended precision to calculate reduced argument.
			Split 2pi in 2 parts a1 and a2, of which the first only
			uses some bits of the mantissa, so that n * a1 is
			exactly representable, where n is the integer part of
			x/pi.
			Here we used 12 bits of the mantissa for a1.
			Also split x in integer part x1 and fraction part x2.
			We then compute x-n*2pi as ((x1 - n*a1) + x2) - n*a2.
		*/
#define A1 6.2822265625
#define A2 0.00095874467958647692528676655900576
		double n = (long) (x / M_2PI);
		double x1 = (long) x;
		double x2 = x - x1;
		x = x1 - n * A1;
		x += x2;
		x -= n * A2;
#undef A1
#undef A2
	    }
	    else {
		extern double _fif();
		double dummy;

		x = _fif(x/M_2PI, 1.0, &dummy) * M_2PI;
	    }
	}
	x /= M_PI_2;
	t = x;
	x -= t;
	quadrant = (quadrant + (int)(t % 4)) % 4;
	if (quadrant & 01) {
		x = 1 - x;
	}
	if (quadrant > 1) {
		x = -x;
	}
	xsqr = x * x;
	x = x * POLYNOM5(xsqr, p) / POLYNOM5(xsqr, q);
	return x;
}

double
_sin(x)
	double x;
{
	return sinus(x, 0);
}

double
_cos(x)
	double x;
{
	if (x < 0) x = -x;
	return sinus(x, 1);
}
