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

extern double	_fif();
extern double	_fef();
extern		_trp();

/*
	exp returns the exponential function of its
	floating-point argument.

	The coefficients are #1069 from Hart and Cheney. (22.35D)
*/

#define	HUGE	1.701411733192644270e38

static double p0	=  .2080384346694663001443843411e7;
static double p1	=  .3028697169744036299076048876e5;
static double p2	=  .6061485330061080841615584556e2;
static double q0	=  .6002720360238832528230907598e7;
static double q1	=  .3277251518082914423057964422e6;
static double q2	=  .1749287689093076403844945335e4;
static double log2e	= 1.4426950408889634073599247;
static double sqrt2	= 1.4142135623730950488016887;
static double maxf	= 10000.0;

static double
floor(d)
double d;
{
	if (d<0) {
		d = -d;
		if (_fif(d, 1.0, &d) != 0)
			d += 1;
		d = -d;
	} else
		_fif(d, 1.0, &d);
	return(d);
}

static double
ldexp(fr,exp)
double fr;
int exp;
{
	int	neg,i;

	neg = 1;
	if (fr < 0) {
		fr = -fr;
		neg = -1;
	}
	fr = _fef(fr, &i);
	/*
	while (fr < 0.5) {
		fr *= 2;
		exp--;
	}
	*/
	exp += i;
	if (exp > 127) {
		_trp(EEXP);
		return(neg * HUGE);
	}
	if (exp < -127)
		return(0);
	while (exp > 14) {
		fr *= (1<<14);
		exp -= 14;
	}
	while (exp < -14) {
		fr /= (1<<14);
		exp += 14;
	}
	if (exp > 0)
		fr *= (1<<exp);
	if (exp < 0)
		fr /= (1<<(-exp));
	return(neg * fr);
}

double
_exp(arg)
double arg;
{
	double fract;
	double temp1, temp2, xsq;
	int ent;

	if(arg == 0)
		return(1);
	if(arg < -maxf)
		return(0);
	if(arg > maxf) {
		_trp(EEXP);
		return(HUGE);
	}
	arg *= log2e;
	ent = floor(arg);
	fract = (arg-ent) - 0.5;
	xsq = fract*fract;
	temp1 = ((p2*xsq+p1)*xsq+p0)*fract;
	temp2 = ((xsq+q2)*xsq+q1)*xsq + q0;
	return(ldexp(sqrt2*(temp2+temp1)/(temp2-temp1), ent));
}
