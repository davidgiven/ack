/*
 * Sources of the "FLOATING POINT ARITHMETIC" group instructions
 */

/* $Header$ */

#include	<em_abs.h>
#include	"nofloat.h"
#include	"global.h"
#include	"log.h"
#include	"mem.h"
#include	"trap.h"
#include	"text.h"
#include	"fra.h"
#include	"warn.h"

#ifndef	NOFLOAT

extern double fpop();

#define	MAXDOUBLE	99.e999		/* IEEE infinity */	/*???*/
#define	SMALL		(1.0/MAXDOUBLE)

PRIVATE double adf(), sbf(), mlf(), dvf();
PRIVATE double ttttp();
PRIVATE double floor(), fabs();
PRIVATE fef(), fif();

#endif	NOFLOAT

DoADFl2(arg)
	size arg;
{
	/* ADF w: Floating add (*) */
#ifndef	NOFLOAT
	register size l = (L_arg_2() * arg);
	double t = fpop(arg_wf(l));

	LOG(("@F6 DoADFl2(%ld)", l));
	spoilFRA();
	fpush(adf(fpop(l), t), l);
#else	NOFLOAT
	arg = arg;
	nofloat();
#endif	NOFLOAT
}

DoADFs(hob, wfac)
	long hob;
	size wfac;
{
	/* ADF w: Floating add (*) */
#ifndef	NOFLOAT
	register size l = (S_arg(hob) * wfac);
	double t = fpop(arg_wf(l));

	LOG(("@F6 DoADFs(%ld)", l));
	spoilFRA();
	fpush(adf(fpop(l), t), l);
#else	NOFLOAT
	hob = hob;
	wfac = wfac;
	nofloat();
#endif	NOFLOAT
}

DoADFz()
{
	/* ADF w: Floating add (*) */
#ifndef	NOFLOAT
	register size l = upop(wsize);
	double t = fpop(arg_wf(l));

	LOG(("@F6 DoADFz(%ld)", l));
	spoilFRA();
	fpush(adf(fpop(l), t), l);
#else	NOFLOAT
	nofloat();
#endif	NOFLOAT
}

DoSBFl2(arg)
	size arg;
{
	/* SBF w: Floating subtract (*) */
#ifndef	NOFLOAT
	register size l = (L_arg_2() * arg);
	double t = fpop(arg_wf(l));

	LOG(("@F6 DoSBFl2(%ld)", l));
	spoilFRA();
	fpush(sbf(fpop(l), t), l);
#else	NOFLOAT
	arg = arg;
	nofloat();
#endif	NOFLOAT
}

DoSBFs(hob, wfac)
	long hob;
	size wfac;
{
	/* SBF w: Floating subtract (*) */
#ifndef	NOFLOAT
	register size l = (S_arg(hob) * wfac);
	double t = fpop(arg_wf(l));

	LOG(("@F6 DoSBFs(%ld)", l));
	spoilFRA();
	fpush(sbf(fpop(l), t), l);
#else	NOFLOAT
	hob = hob;
	wfac = wfac;
	nofloat();
#endif	NOFLOAT
}

DoSBFz()
{
	/* SBF w: Floating subtract (*) */
#ifndef	NOFLOAT
	register size l = upop(wsize);
	double t = fpop(arg_wf(l));

	LOG(("@F6 DoSBFz(%ld)", l));
	spoilFRA();
	fpush(sbf(fpop(l), t), l);
#else	NOFLOAT
	nofloat();
#endif	NOFLOAT
}

DoMLFl2(arg)
	size arg;
{
	/* MLF w: Floating multiply (*) */
#ifndef	NOFLOAT
	register size l = (L_arg_2() * arg);
	double t = fpop(arg_wf(l));

	LOG(("@F6 DoMLFl2(%ld)", l));
	spoilFRA();
	fpush(mlf(fpop(l), t), l);
#else	NOFLOAT
	arg = arg;
	nofloat();
#endif	NOFLOAT
}

DoMLFs(hob, wfac)
	long hob;
	size wfac;
{
	/* MLF w: Floating multiply (*) */
#ifndef	NOFLOAT
	register size l = (S_arg(hob) * wfac);
	double t = fpop(arg_wf(l));

	LOG(("@F6 DoMLFs(%ld)", l));
	spoilFRA();
	fpush(mlf(fpop(l), t), l);
#else	NOFLOAT
	hob = hob;
	wfac = wfac;
	nofloat();
#endif	NOFLOAT
}

DoMLFz()
{
	/* MLF w: Floating multiply (*) */
#ifndef	NOFLOAT
	register size l = upop(wsize);
	double t = fpop(arg_wf(l));

	LOG(("@F6 DoMLFz(%ld)", l));
	spoilFRA();
	fpush(mlf(fpop(l), t), l);
#else	NOFLOAT
	nofloat();
#endif	NOFLOAT
}

DoDVFl2(arg)
	size arg;
{
	/* DVF w: Floating divide (*) */
#ifndef	NOFLOAT
	register size l = (L_arg_2() * arg);
	double t = fpop(arg_wf(l));

	LOG(("@F6 DoDVFl2(%ld)", l));
	spoilFRA();
	fpush(dvf(fpop(l), t), l);
#else	NOFLOAT
	arg = arg;
	nofloat();
#endif	NOFLOAT
}

DoDVFs(hob, wfac)
	long hob;
	size wfac;
{
	/* DVF w: Floating divide (*) */
#ifndef	NOFLOAT
	register size l = (S_arg(hob) * wfac);
	double t = fpop(arg_wf(l));

	LOG(("@F6 DoDVFs(%ld)", l));
	spoilFRA();
	fpush(dvf(fpop(l), t), l);
#else	NOFLOAT
	hob = hob;
	wfac = wfac;
	nofloat();
#endif	NOFLOAT
}

DoDVFz()
{
	/* DVF w: Floating divide (*) */
#ifndef	NOFLOAT
	register size l = upop(wsize);
	double t = fpop(arg_wf(l));

	LOG(("@F6 DoDVFz(%ld)", l));
	spoilFRA();
	fpush(dvf(fpop(l), t), l);
#else	NOFLOAT
	nofloat();
#endif	NOFLOAT
}

DoNGFl2(arg)
	size arg;
{
	/* NGF w: Floating negate (*) */
#ifndef	NOFLOAT
	register size l = (L_arg_2() * arg);
	double t = fpop(arg_wf(l));

	LOG(("@F6 DoNGFl2(%ld)", l));
	spoilFRA();
	fpush(-t, l);
#else	NOFLOAT
	arg = arg;
	nofloat();
#endif	NOFLOAT
}

DoNGFz()
{
	/* NGF w: Floating negate (*) */
#ifndef	NOFLOAT
	register size l = upop(wsize);
	double t = fpop(arg_wf(l));

	LOG(("@F6 DoNGFz(%ld)", l));
	spoilFRA();
	fpush(-t, l);
#else	NOFLOAT
	nofloat();
#endif	NOFLOAT
}

DoFIFl2(arg)
	size arg;
{
	/* FIF w: Floating multiply and split integer and fraction part (*) */
#ifndef	NOFLOAT
	register size l = (L_arg_2() * arg);
	double t = fpop(arg_wf(l));

	LOG(("@F6 DoFIFl2(%ld)", l));
	spoilFRA();
	fif(fpop(l), t, l);
#else	NOFLOAT
	arg = arg;
	nofloat();
#endif	NOFLOAT
}

DoFIFz()
{
	/* FIF w: Floating multiply and split integer and fraction part (*) */
#ifndef	NOFLOAT
	register size l = upop(wsize);
	double t = fpop(arg_wf(l));

	LOG(("@F6 DoFIFz(%ld)", l));
	spoilFRA();
	fif(fpop(l), t, l);
#else	NOFLOAT
	nofloat();
#endif	NOFLOAT
}

DoFEFl2(arg)
	size arg;
{
	/* FEF w: Split floating number in exponent and fraction part (*) */
#ifndef	NOFLOAT
	register size l = (L_arg_2() * arg);

	LOG(("@F6 DoFEFl2(%ld)", l));
	spoilFRA();
	fef(fpop(arg_wf(l)), l);
#else	NOFLOAT
	arg = arg;
	nofloat();
#endif	NOFLOAT
}

DoFEFz()
{
	/* FEF w: Split floating number in exponent and fraction part (*) */
#ifndef	NOFLOAT
	register size l = upop(wsize);

	LOG(("@F6 DoFEFz(%ld)", l));
	spoilFRA();
	fef(fpop(arg_wf(l)), l);
#else	NOFLOAT
	nofloat();
#endif	NOFLOAT
}

#ifndef	NOFLOAT

/* Service routines */

PRIVATE double adf(f1, f2)		/* returns f1 + f2 */
	double f1, f2;
{
	if (must_test && !(IgnMask&BIT(EFOVFL))) {
		if (f1 > 0.0 && f2 > 0.0) {
			if (MAXDOUBLE - f1 < f2) {
				trap(EFOVFL);
				return (0.0);
			}
		}
		else if (f1 < 0.0 && f2 < 0.0) {
			if (-(MAXDOUBLE + f1) > f2) {
				trap(EFOVFL);
				return (0.0);
			}
		}
	}
	return (f1 + f2);
}

PRIVATE double sbf(f1, f2)		/* returns f1 - f2 */
	double f1, f2;
{
	if (must_test && !(IgnMask&BIT(EFOVFL))) {
		if (f2 < 0.0 && f1 > 0.0) {
			if (MAXDOUBLE - f1 < -f2) {
				trap(EFOVFL);
				return (0.0);
			}
		}
		else if (f2 > 0.0 && f1 < 0.0) {
			if (f2 - MAXDOUBLE > f1) {
				trap(EFOVFL);
				return (0.0);
			}
		}
	}
	return (f1 - f2);
}

PRIVATE double mlf(f1, f2)		/* returns f1 * f2 */
	double f1, f2;
{
	double ff1 = fabs(f1), ff2 = fabs(f2);

	if (f1 == 0.0 || f2 == 0.0)
		return (0.0);

	if ((ff1 >= 1.0 && ff2 <= 1.0) || (ff2 >= 1.0 && ff1 <= 1.0))
		return (f1 * f2);

	if (must_test && !(IgnMask&BIT(EFUNFL))) {
		if (ff1 < 1.0 && ff2 < 1.0) {
			if (SMALL / ff1 > ff2) {
				trap(EFUNFL);
				return (0.0);
			}
			return (f1 * f2);
		}
	}

	if (must_test && !(IgnMask&BIT(EFOVFL))) {
		if (MAXDOUBLE / ff1 < ff2) {
			trap(EFOVFL);
			return (0.0);
		}
	}
	return (f1 * f2);
}

PRIVATE double dvf(f1, f2)		/* returns f1 / f2 */
	double f1, f2;
{
	double ff1 = fabs(f1), ff2 = fabs(f2);

	if (f2 == 0.0) {
		if (!(IgnMask&BIT(EFDIVZ))) {
			trap(EFDIVZ);
		}
		else	return (0.0);
	}

	if (f1 == 0.0)
		return (0.0);

	if ((ff2 >= 1.0 && ff1 >= 1.0) || (ff1 <= 1.0 && ff2 <= 1.0))
		return (f1 / f2);

	if (must_test && !(IgnMask&BIT(EFUNFL))) {
		if (ff2 > 1.0 && ff1 < 1.0) {
			if (SMALL / ff2 > ff1) {
				trap(EFUNFL);
				return (0.0);
			}
			return (f1 / f2);
		}
	}

	if (must_test && !(IgnMask&BIT(EFOVFL))) {
		if (MAXDOUBLE * ff2  < ff1) {
			trap(EFOVFL);
			return (0.0);
		}
	}
	return (f1 / f2);
}

PRIVATE fif(f1, f2, n)
	double f1, f2;
	size n;
{
	double f = mlf(f1, f2);
	double fl = floor(fabs(f));

	fpush(fabs(f) - fl, n);		/* push fraction */
	fpush((f < 0.0) ? -fl : fl, n);	/* push integer-part */
}

PRIVATE fef(f, n)
	double f;
	size n;
{
	register long exponent, sign = (long) (f < 0.0);

	for (f = fabs(f), exponent = 0; f >= 1.0; exponent++)
		f /= 2.0;

	for (; f < 0.5; exponent--)
		f *= 2.0;

	fpush((sign) ? -f : f, n);	/* push mantissa */
	wpush(exponent);		/* push exponent */
}

/* floating point service routines, to avoid having to use -lm */

PRIVATE double fabs(f)
	double f;
{
	return (f < 0.0 ? -f : f);
}

PRIVATE double floor(f)
	double f;
{
	double res, d;
	register int sign = 1;
	
	/* eliminate the sign */
	if (f < 0) {
		sign = -1, f = -f;
	}
	
	/* get the largest power of 2 <= f */
	d = 1.0;
	while (f - d >= d) {
		d *= 2.0;
	}
	
	/* reconstruct f by deminishing powers of 2 */
	res = 0.0;
	while (d >= 1.0) {
		if (res + d <= f)
			res += d;
		d /= 2.0;
	}
	
	/* undo the sign elimination */
	if (sign == -1) {
		res = -res, f = -f;
		if (res > f)
			res -= 1.0;
	}
	
	return res;
}

PRIVATE double ttttp(f, n)		/* times ten to the power */
	double f;
{
	while (n > 0) {
		f = mlf(f, 10.0);
		n--;
	}
	while (n < 0) {
		f = dvf(f, 10.0);
		n++;
	}
	return f;
}

/*	Str2double is used to initialize the global data area with floats;
	we do not use, e.g., sscanf(), to be able to check the grammar of
	the string and to give warnings.
*/

double str2double(str)
	char *str;
{
	register char b;
	register int sign = 1;		/* either +1 or -1 */
	register int frac = 0;		/* how far in fraction part ? */
	register int ex;		/* to store exponent */
	double mantissa = 0.0;		/* to store mantissa */
	double d;			/* double to be returned */
	
	b = *str++;
	if (b == '-') {
		sign = -1;
		b = *str++;
	}
	else if (b == '+') {
		sign = 1;
		b = *str++;
	}
	
	if ('0' <= b && b <= '9') {
		mantissa = (double) (b-'0');
	}
	else if (b == '.') {
		/* part before dot cannot be empty */
		warning(WBADFLOAT);
		frac = 1;
	}
	else {
		goto BadFloat;
	}
	
	LOG((" q9 str2double : (before while) mantissa = %20.20g", mantissa));
	
	while ((b = *str++) != 'e' && b != 'E' && b != '\0') {
		if (b == '.') {
			if (frac == 0) {
				frac++;
			}
			else {	/* there already was a '.' in input */
				goto BadFloat;
			}
		}
		else if ('0' <= b && b <= '9') {
			double bval = b - '0';
			
			if (frac) {
				mantissa =
					adf(mantissa, ttttp(bval, -frac));
				frac++;
			}
			else {
				mantissa =
					adf(mlf(mantissa, 10.0), bval);
			}
		}
		else {
			goto BadFloat;
		}
		LOG((" q9 str2double : (inside while) mantissa = %20.20g",
								mantissa));
	}
	LOG((" q9 str2double : mantissa = %10.10g", mantissa));
	mantissa = sign * mantissa;
	if (b == '\0')
		return (mantissa);
	/* else we have b == 'e' or b== 'E' */
	
	/* Optional sign for exponent */
	b = *str++;
	if (b == '-') {
		sign = -1;
		b = *str++;
	}
	else if (b == '+') {
		sign = 1;
		b = *str++;
	}
	else {
		sign = 1;
	}
	
	ex = 0;
	do {
		if ('0' <= b && b <= '9') {
			ex = 10*ex + (b-'0');
		}
		else {
			goto BadFloat;
		}
	} while ((b = *str++) != '\0');
	LOG((" q9 str2double : exponent = %d", ex));
	
	/* Construct total value of float */
	ex = sign * ex;
	d = ttttp(mantissa, ex);
	return (d);

BadFloat:
	fatal("Float garbled in loadfile");
	return (0.0);
}

#else	NOFLOAT

nofloat() {
	fatal("attempt to execute a floating point instruction on an EM machine without FP");
}

#endif	NOFLOAT

