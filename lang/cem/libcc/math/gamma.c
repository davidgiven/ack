/*
 * (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* $Id$ */

#include <math.h>
#include <errno.h>

extern int errno;

static double
smallpos_gamma(x)
	double x;
{
	/*	Approximation of gamma function using
		gamma(x) = P(x-2) / Q(x-2) for x in [2,3]
	*/
	/* Hart & Cheney # 5251 */

	static double p[11] = {
		-0.2983543278574342138830437659e+06,
		-0.2384953970018198872468734423e+06,
		-0.1170494760121780688403854445e+06,
		-0.3949445048301571936421824091e+05,
		-0.1046699423827521405330650531e+05,
		-0.2188218110071816359394795998e+04,
		-0.3805112208641734657584922631e+03,
		-0.5283123755635845383718978382e+02,
		-0.6128571763704498306889428212e+01,
		-0.5028018054416812467364198750e+00,
		-0.3343060322330595274515660112e-01
	};

	static double q[9] = {
		-0.2983543278574342138830438524e+06,
		-0.1123558608748644911342306408e+06,
		 0.5332716689118142157485686311e+05,
		 0.8571160498907043851961147763e+04,
		-0.4734865977028211706556819770e+04,
		 0.1960497612885585838997039621e+03,
		 0.1257733367869888645966647426e+03,
		-0.2053126153100672764513929067e+02,
		 0.1000000000000000000000000000e+01
	};

	double result = 1.0;

	while (x > 3) {
		x -= 1.0;
		result *= x;
	}
	while (x < 2) {
		result /= x;
		x += 1.0;
	}

	x -= 2.0;

	return result * POLYNOM10(x, p) / POLYNOM8(x, q);
}

#define log_sqrt_2pi 0.91893853320467274178032973640561763

int	signgam;

static double
bigpos_loggamma(x)
	double x;
{
	/*	computes the log(gamma(x)) function for big arguments
		using the Stirling form
		  log(gamma(x)) = (x - 0.5)log(x) - x + log(sqrt(2*pi)) + fi(x)
		where fi(x) = (1/x)*P(1/(x*x))/Q(1/(x*x)) for x in [12,1000]
	*/
	/* Hart & Cheney # 5468 */

	static double p[4] = {
		 0.12398282342474941538685913e+00,
		 0.67082783834332134961461700e+00,
		 0.64507302912892202513890000e+00,
		 0.66662907040200752600000000e-01
	};

	static double q[4] = {
		 0.14877938810969929846815600e+01,
		 0.80995271894897557472821400e+01,
		 0.79966911236636441947720000e+01,
		 0.10000000000000000000000000e+01
	};

	double rsq = 1.0/(x*x);
	extern double log();

	return (x-0.5)*log(x)-x+log_sqrt_2pi+POLYNOM3(rsq, p)/(x*POLYNOM3(rsq, q));
}

static double
neg_loggamma(x)
	double x;
{
	/*	compute the log(gamma(x)) function for negative values of x,
		using the rule:
			-x*gamma(x)*gamma(-x) = pi/sin(z*pi)
	*/
	extern double sin(), log();
	double sinpix;

	x = -x;
	sinpix = sin(M_PI * x);
	if (sinpix == 0.0) {
		errno = EDOM;
		return HUGE;
	}
	if (sinpix < 0) sinpix = -sinpix;
	else signgam = -1;
	return log(M_PI/(x * smallpos_gamma(x) * sinpix));
}

double
gamma(x)
	double x;
{
	/*	Wrong name; Actually computes log(gamma(x))
	*/
	extern double log();

	signgam = 1;
	if (x <= 0) {
		return neg_loggamma(x);
	}
	if (x > 12.0) {
		return bigpos_loggamma(x);
	}
	return log(smallpos_gamma(x));
}
