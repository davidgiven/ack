/*
 * (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

#include <math.h>
#include <stdio.h>

#define EPS_D	5.0e-14
main()
{
	testsqrt();
	testtrig();
	testexplog();
	testgamma();
	testbessel();
}

dotest(s, x, d, v)
	char *s;
	double x, d, v;
{
	double fabs();

	if (fabs((v - d) / (fabs(v) < EPS_D ? 1.0 : v)) > EPS_D) {
		printf(s, x);
		printf(" = %.16e, should be %.16e\n", d, v);
	}
}

testsqrt()
{
#define SQRT2	M_SQRT2
#define SQRT10	3.16227766016837933199889354443271853

	double x, val;
	extern double sqrt();

	dotest("sqrt(%.1f)", 2.0, sqrt(2.0), SQRT2);
	dotest("sqrt(%.1f)", 10.0, sqrt(10.0), SQRT10);

	for (x = 0.1; x < 0.1e20; x += x) {
		val = sqrt(x);
		dotest("sqrt(%.1f)^2", x, val*val, x);
	}
}

testtrig()
{
#define SINPI_24	0.13052619222005159154840622789548901
#define SINPI_16	0.19509032201612826784828486847702224
#define SINPI_12	0.25881904510252076234889883762404832
#define SINPI_6		0.5
#define SINPI_4		M_1_SQRT2
#define SINPI_3		0.86602540378443864676372317075293618
#define SINPI_2		1.0
#define SIN0		0.0

	double x;
	extern double sin(), cos(), tan(), asin(), acos(), atan(), fabs();

	dotest("sin(0)", 0.0, sin(0.0), SIN0);
	dotest("sin(pi/24)", M_PI/24 , sin(M_PI/24), SINPI_24);
	dotest("sin(pi/16)", M_PI/16 , sin(M_PI/16), SINPI_16);
	dotest("sin(pi/12)", M_PI/12 , sin(M_PI/12), SINPI_12);
	dotest("sin(pi/6)", M_PI/6 , sin(M_PI/6), SINPI_6);
	dotest("sin(pi/4)", M_PI_4 , sin(M_PI_4), SINPI_4);
	dotest("sin(pi/3)", M_PI/3 , sin(M_PI/3), SINPI_3);
	dotest("sin(pi/2)", M_PI_2 , sin(M_PI_2), SINPI_2);
	dotest("sin(pi)", 0.0, sin(M_PI), SIN0);
	dotest("sin(3*pi/2)", 0.0, sin(M_PI+M_PI_2), -SINPI_2);

	dotest("sin(-pi/24)", -M_PI/24 , sin(-M_PI/24), -SINPI_24);
	dotest("sin(-pi/16)", -M_PI/16 , sin(-M_PI/16), -SINPI_16);
	dotest("sin(-pi/12)", -M_PI/12 , sin(-M_PI/12), -SINPI_12);
	dotest("sin(-pi/6)", -M_PI/6 , sin(-M_PI/6), -SINPI_6);
	dotest("sin(-pi/4)", -M_PI_4 , sin(-M_PI_4), -SINPI_4);
	dotest("sin(-pi/3)", -M_PI/3 , sin(-M_PI/3), -SINPI_3);
	dotest("sin(-pi/2)", -M_PI_2 , sin(-M_PI_2), -SINPI_2);

	dotest("cos(pi/2)", M_PI_2, cos(M_PI_2), SIN0);
	dotest("cos(11pi/24)", M_PI/24 , cos(11*M_PI/24), SINPI_24);
	dotest("cos(7pi/16)", M_PI/16 , cos(7*M_PI/16), SINPI_16);
	dotest("cos(5pi/12)", M_PI/12 , cos(5*M_PI/12), SINPI_12);
	dotest("cos(pi/3)", M_PI/6 , cos(M_PI/3), SINPI_6);
	dotest("cos(pi/4)", M_PI_4 , cos(M_PI_4), SINPI_4);
	dotest("cos(pi/6)", M_PI/3 , cos(M_PI/6), SINPI_3);
	dotest("cos(0)", M_PI_2 , cos(0), SINPI_2);
	dotest("cos(pi)", M_PI , cos(M_PI), -SINPI_2);
	dotest("cos(3pi/2)", M_PI , cos(M_PI+M_PI_2), SIN0);

	dotest("cos(-pi/2)", M_PI_2, cos(-M_PI_2), SIN0);
	dotest("cos(-11pi/24)", M_PI/24 , cos(-11*M_PI/24), SINPI_24);
	dotest("cos(-7pi/16)", M_PI/16 , cos(-7*M_PI/16), SINPI_16);
	dotest("cos(-5pi/12)", M_PI/12 , cos(-5*M_PI/12), SINPI_12);
	dotest("cos(-pi/3)", M_PI/6 , cos(-M_PI/3), SINPI_6);
	dotest("cos(-pi/4)", M_PI_4 , cos(-M_PI_4), SINPI_4);
	dotest("cos(-pi/6)", M_PI/3 , cos(-M_PI/6), SINPI_3);

	for (x = -10; x <= 10; x += 0.5) {
		dotest("sin+2*pi-sin(%.2f)", x, sin(x+M_2PI)-sin(x), 0.0);
		dotest("cos+2*pi-cos(%.2f)", x, cos(x+M_2PI)-cos(x), 0.0);
		dotest("tan+2*pi-tan(%.2f)", x, tan(x+M_2PI)-tan(x), 0.0);
		dotest("tan+pi-tan(%.2f)", x, tan(x+M_PI)-tan(x), 0.0);
	}

	for (x = -1.5; x <= 1.5; x += 0.1) {
		dotest("asin(sin(%.2f))", x, asin(sin(x)), x);
		dotest("acos(cos(%.2f))", x, acos(cos(x)), fabs(x));
		dotest("atan(tan(%.2f))", x, atan(tan(x)), x);
	}
}

testexplog()
{
#define EXPMIN1		0.36787944117144232159552377016146087	/* exp(-1) */
#define EXPMIN1_4	0.77880078307140486824517026697832065	/* exp(-1/4) */
#define EXP0		1.0					/* exp(0) */
#define EXP1_4		1.28402541668774148407342056806243646	/* exp(1/4) */
#define EXP1		M_E					/* exp(1) */
#define LN1		0.0					/* log(1) */
#define LN2		M_LN2					/* log(2) */
#define LN4		1.38629436111989061883446424291635313	/* log(4) */
#define LNE		1.0					/* log(e) */
#define LN10		M_LN10					/* log(10) */

	extern double exp(), log();
	double x;

	dotest("exp(%.2f)", -1.0, exp(-1.0), EXPMIN1);
	dotest("exp(%.2f)", -0.25, exp(-0.25), EXPMIN1_4);
	dotest("exp(%.2f)", 0.0, exp(0.0), EXP0);
	dotest("exp(%.2f)", 0.25, exp(0.25), EXP1_4);
	dotest("exp(%.2f)", 1.0, exp(1.0), EXP1);

	dotest("log(%.2f)", 1.0, log(1.0), LN1);
	dotest("log(%.2f)", 2.0, log(2.0), LN2);
	dotest("log(%.2f)", 4.0, log(4.0), LN4);
	dotest("log(%.2f)", 10.0, log(10.0), LN10);
	dotest("log(e)", M_E, log(M_E), LNE);

	for (x = -30.0; x <= 30.0; x += 0.5) {
		dotest("log(exp(%.2f))", x, log(exp(x)), x);
	}
}

testgamma()
{
	double x, xfac;
	extern double gamma(), exp();

	for (x = 1.0, xfac = 1.0; x < 30.0; x += 1.0) {
		dotest("exp(gamma(%.2f))", x, exp(gamma(x)), xfac);
		xfac *= x;
	}
}

testbessel()
{
#define J0__PI_4	0.85163191370480801270040601506092607 /* j0(pi/4) */
#define J0__PI_2	0.47200121576823476744766838787250096 /* j0(pi/2) */
#define J1__PI_4	0.36318783834686733179559374778892472 /* j1(pi/4) */
#define J1__PI_2	0.56682408890587393771124496346716028 /* j1(pi/2) */
#define J10__PI_4	0.00000000002369974904082422018721148 /* j10(p1/4) */
#define J10__PI_2	0.00000002326614794865976450546482206 /* j10(pi/2) */

	extern double j0(), j1(), jn(), yn();
	register int n;
	double x;
	extern char *sprintf();
	char buf[100];

	dotest("j0(pi/4)", M_PI_4, j0(M_PI_4), J0__PI_4);
	dotest("j0(pi/2)", M_PI_2, j0(M_PI_2), J0__PI_2);
	dotest("j1(pi/4)", M_PI_4, j1(M_PI_4), J1__PI_4);
	dotest("j1(pi/2)", M_PI_2, j1(M_PI_2), J1__PI_2);
	dotest("j10(pi/4)", M_PI_4, jn(10,M_PI_4), J10__PI_4);
	dotest("j10(pi/2)", M_PI_2, jn(10,M_PI_2), J10__PI_2);

	/* Also check consistency using the Wronskian relation
		jn(n+1,x)*yn(n, x) - jn(n,x)*yn(n+1,x) = 2/(pi*x)
	*/

	for (x = 0.1; x < 20.0; x += 0.5) {
		double two_over_pix = M_2_PI/x;

		for (n = 0; n <= 10; n++) {
			dotest(sprintf(buf, "jn(%d,%.2f)*yn(%d,%.2f)-jn(%d,%.2f)*yn(%d,%.2f)",n+1,x,n,x,n,x,n+1,x), x, jn(n+1,x)*yn(n,x)-jn(n,x)*yn(n+1,x),M_2_PI/x);
		}
	}
}
