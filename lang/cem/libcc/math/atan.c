/*
 * (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* $Header$ */

#include <math.h>
#include <errno.h>

double
atan(x)
	double x;
{
	/*	The interval [0, infinity) is treated as follows:
		Define partition points Xi
			X0 = 0
			X1 = tan(pi/16)
			X2 = tan(3pi/16)
			X3 = tan(5pi/16)
			X4 = tan(7pi/16)
			X5 = infinity
		and evaluation nodes xi
			x2 = tan(2pi/16)
			x3 = tan(4pi/16)
			x4 = tan(6pi/16)
			x5 = infinity
		An argument x in [Xn-1, Xn] is now reduced to an argument
		t in [-X1, X1] by the following formulas:
			
			t = 1/xn - (1/(xn*xn) + 1)/((1/xn) + x)

			arctan(x) = arctan(xi) + arctan(t)

		For the interval [0, p/16] an approximation is used:
			arctan(x) = x * P(x*x)/Q(x*x)
	*/
	static struct precomputed {
		double X;		/* partition point */
		double arctan;		/* arctan of evaluation node */
		double one_o_x;		/* 1 / xn */
		double one_o_xsq_p_1;	/* 1 / (xn*xn) + 1 */
	} prec[5] = {
		{ 0.19891236737965800691159762264467622,
		  0.0,
		  0.0,		/* these don't matter */
		  0.0 } ,
		{ 0.66817863791929891999775768652308076, /* tan(3pi/16)	*/
		  M_PI_8,
		  2.41421356237309504880168872420969808,
		  6.82842712474619009760337744841939616 },
		{ 1.49660576266548901760113513494247691, /* tan(5pi/16) */
		  M_PI_4,
		  1.0,
		  2.0 },
		{ 5.02733949212584810451497507106407238, /* tan(7pi/16) */
		  M_3PI_8,
		  0.41421356237309504880168872420969808,
		  1.17157287525380998659662255158060384 },
		{ MAXDOUBLE,
		  M_PI_2,
		  0.0,
		  1.0 }};

	/*	Hart & Cheney # 5037 */

	static double p[5] = {
		0.7698297257888171026986294745e+03,
		0.1557282793158363491416585283e+04,
		0.1033384651675161628243434662e+04,
		0.2485841954911840502660889866e+03,
		0.1566564964979791769948970100e+02
	};

	static double q[6] = {
		0.7698297257888171026986294911e+03,
		0.1813892701754635858982709369e+04,
		0.1484049607102276827437401170e+04,
		0.4904645326203706217748848797e+03,
		0.5593479839280348664778328000e+02,
		0.1000000000000000000000000000e+01
	};

	int negative = x < 0.0;
	register struct precomputed *pr = prec;

	if (negative) {
		x = -x;
	}
	while (x > pr->X) pr++;
	if (pr != prec) {
		x = pr->arctan +
			atan(pr->one_o_x - pr->one_o_xsq_p_1/(pr->one_o_x + x));
	}
	else {
		double xsq = x*x;

		x = x * POLYNOM4(xsq, p)/POLYNOM5(xsq, q);
	}
	return negative ? -x : x;
}
