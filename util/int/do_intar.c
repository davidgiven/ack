/*
 * Sources of the "INTEGER ARITHMETIC" group instructions
 */

/* $Header$ */

#include	<em_abs.h>
#include	"logging.h"
#include	"global.h"
#include	"log.h"
#include	"mem.h"
#include	"trap.h"
#include	"warn.h"
#include	"text.h"
#include	"fra.h"

PRIVATE long adi(), sbi(), dvi(), mli(), rmi(), ngi(), sli(), sri();

DoADIl2(arg)
	size arg;
{
	/* ADI w: Addition (*) */
	register size l = (L_arg_2() * arg);
	register long t = spop(arg_wi(l));

	LOG(("@I6 DoADIl2(%ld)", l));
	spoilFRA();
	npush(adi(spop(l), t, l), l);
}

DoADIm(arg)
	size arg;
{
	/* ADI w: Addition (*) */
	register size l = arg_wi(arg);
	register long t = spop(l);

	LOG(("@I6 DoADIm(%ld)", l));
	spoilFRA();
	npush(adi(spop(l), t, l), l);
}

DoADIz()				/* argument on top of stack */
{
	/* ADI w: Addition (*) */
	register size l = upop(wsize);
	register long t = spop(arg_wi(l));

	LOG(("@I6 DoADIz(%ld)", l));
	spoilFRA();
	npush(adi(spop(l), t, l), l);
}

DoSBIl2(arg)
	size arg;
{
	/* SBI w: Subtraction (*) */
	register size l = (L_arg_2() * arg);
	register long t = spop(arg_wi(l));

	LOG(("@I6 DoSBIl2(%ld)", l));
	spoilFRA();
	npush(sbi(spop(l), t, l), l);
}

DoSBIm(arg)
	size arg;
{
	/* SBI w: Subtraction (*) */
	register size l = arg_wi(arg);
	register long t = spop(l);

	LOG(("@I6 DoSBIm(%ld)", l));
	spoilFRA();
	npush(sbi(spop(l), t, l), l);
}

DoSBIz()				/* arg on top of stack */
{
	/* SBI w: Subtraction (*) */
	register size l = upop(wsize);
	register long t = spop(arg_wi(l));

	LOG(("@I6 DoSBIz(%ld)", l));
	spoilFRA();
	npush(sbi(spop(l), t, l), l);
}

DoMLIl2(arg)
	size arg;
{
	/* MLI w: Multiplication (*) */
	register size l = (L_arg_2() * arg);
	register long t = spop(arg_wi(l));

	LOG(("@I6 DoMLIl2(%ld)", l));
	spoilFRA();
	npush(mli(spop(l), t, l), l);
}

DoMLIm(arg)
	size arg;
{
	/* MLI w: Multiplication (*) */
	register size l = arg_wi(arg);
	register long t = spop(l);

	LOG(("@I6 DoMLIm(%ld)", l));
	spoilFRA();
	npush(mli(spop(l), t, l), l);
}

DoMLIz()				/* arg on top of stack */
{
	/* MLI w: Multiplication (*) */
	register size l = upop(wsize);
	register long t = spop(arg_wi(l));

	LOG(("@I6 DoMLIz(%ld)", l));
	spoilFRA();
	npush(mli(spop(l), t, l), l);
}

DoDVIl2(arg)
	size arg;
{
	/* DVI w: Division (*) */
	register size l = (L_arg_2() * arg);
	register long t = spop(arg_wi(l));

	LOG(("@I6 DoDVIl2(%ld)", l));
	spoilFRA();
	npush(dvi(spop(l), t), l);
}

DoDVIm(arg)
	size arg;
{
	/* DVI w: Division (*) */
	register size l = arg_wi(arg);
	register long t = spop(l);

	LOG(("@I6 DoDVIm(%ld)", l));
	spoilFRA();
	npush(dvi(spop(l), t), l);
}

DoDVIz()				/* arg on top of stack */
{
	/* DVI w: Division (*) */
	register size l = upop(wsize);
	register long t = spop(arg_wi(l));

	LOG(("@I6 DoDVIz(%ld)", l));
	spoilFRA();
	npush(dvi(spop(l), t), l);
}

DoRMIl2(arg)
	size arg;
{
	/* RMI w: Remainder (*) */
	register size l = (L_arg_2() * arg);
	register long t = spop(arg_wi(l));

	LOG(("@I6 DoRMIl2(%ld)", l));
	spoilFRA();
	npush(rmi(spop(l), t), l);
}

DoRMIm(arg)
	size arg;
{
	/* RMI w: Remainder (*) */
	register size l = arg_wi(arg);
	register long t = spop(l);

	LOG(("@I6 DoRMIm(%ld)", l));
	spoilFRA();
	npush(rmi(spop(l), t), l);
}

DoRMIz()				/* arg on top of stack */
{
	/* RMI w: Remainder (*) */
	register size l = upop(wsize);
	register long t = spop(arg_wi(l));

	LOG(("@I6 DoRMIz(%ld)", l));
	spoilFRA();
	npush(rmi(spop(l), t), l);
}

DoNGIl2(arg)
	size arg;
{
	/* NGI w: Negate (two's complement) (*) */
	register size l = (L_arg_2() * arg);

	LOG(("@I6 DoNGIl2(%ld)", l));
	spoilFRA();
	l = arg_wi(l);
	npush(ngi(spop(l), l), l);
}

DoNGIz()
{
	/* NGI w: Negate (two's complement) (*) */
	register size l = upop(wsize);

	LOG(("@I6 DoNGIz(%ld)", l));
	spoilFRA();
	l = arg_wi(l);
	npush(ngi(spop(l), l), l);
}

DoSLIl2(arg)
	size arg;
{
	/* SLI w: Shift left (*) */
	register size l = (L_arg_2() * arg);
	register long t = spop(wsize);

	LOG(("@I6 DoSLIl2(%ld)", l));
	spoilFRA();
	l = arg_wi(l);
	npush(sli(spop(l), t, l), l);
}

DoSLIm(arg)
	size arg;
{
	/* SLI w: Shift left (*) */
	register size l = arg_wi(arg);
	register long t = spop(wsize);

	LOG(("@I6 DoSLIm(%ld)", l));
	spoilFRA();
	npush(sli(spop(l), t, l), l);
}

DoSLIz()
{
	/* SLI w: Shift left (*) */
	register size l = upop(wsize);
	register long t = spop(wsize);

	LOG(("@I6 DoSLIz(%ld)", l));
	spoilFRA();
	l = arg_wi(l);
	npush(sli(spop(l), t, l), l);
}

DoSRIl2(arg)
	size arg;
{
	/* SRI w: Shift right (*) */
	register size l = (L_arg_2() * arg);
	register long t = spop(wsize);

	LOG(("@I6 DoSRIl2(%ld)", l));
	spoilFRA();
	l = arg_wi(l);
	npush(sri(spop(l), t, l), l);
}

DoSRIz()
{
	/* SRI w: Shift right (*) */
	register size l = upop(wsize);
	register long t = spop(wsize);

	LOG(("@I6 DoSRIz(%ld)", l));
	spoilFRA();
	l = arg_wi(l);
	npush(sri(spop(l), t, l), l);
}

#define	i_maxs(n)		((n == 2) ? I_MAXS2 : I_MAXS4)
#define	i_mins(n)		((n == 2) ? I_MINS2 : I_MINS4)

PRIVATE long adi(w1, w2, nbytes)		/* returns w1 + w2 */
	long w1, w2;
	size nbytes;
{
	if (must_test && !(IgnMask&BIT(EIOVFL))) {
		if (w1 > 0 && w2 > 0) {
			if (i_maxs(nbytes) - w1 < w2)
				trap(EIOVFL);
		}
		else if (w1 < 0 && w2 < 0) {
			if (i_mins(nbytes) - w1 > w2)
				trap(EIOVFL);
		}
	}
	return (w1 + w2);
}

PRIVATE long sbi(w1, w2, nbytes)		/* returns w1 - w2 */
	long w1, w2;
	size nbytes;
{
	if (must_test && !(IgnMask&BIT(EIOVFL))) {
		if (w2 < 0 && w1 > 0) {
			if (i_maxs(nbytes) + w2 < w1)
				trap(EIOVFL);
		}
		else if (w2 > 0 && w1 < 0) {
			if (i_mins(nbytes) + w2 > w1) {
				trap(EIOVFL);
			}
		}
	}
	return (w1 - w2);
}

#define	labs(w)		((w < 0) ? (-w) : w)

PRIVATE long mli(w1, w2, nbytes)		/* returns w1 * w2 */
	long w1, w2;
	size nbytes;
{
	if (w1 == 0 || w2 == 0)
		return (0L);

	if (must_test && !(IgnMask&BIT(EIOVFL))) {
		if ((w1 > 0 && w2 > 0) || (w2 < 0 && w1 < 0)) {
			if (	w1 == i_mins(nbytes) || w2 == i_mins(nbytes)
			||	(i_maxs(nbytes) / labs(w1)) < labs(w2)
			) {
				trap(EIOVFL);
			}
		}
		else if (w1 > 0) {
			if (i_mins(nbytes) / w1 > w2)
				trap(EIOVFL);
		}
		else if (i_mins(nbytes) / w2 > w1) {
			trap(EIOVFL);
		}
	}
	return (w1 * w2);
}

PRIVATE long dvi(w1, w2)
	long w1, w2;
{
	if (w2 == 0) {
		if (!(IgnMask&BIT(EIDIVZ))) {
			trap(EIDIVZ);
		}
		else	return (0L);
	}
	return (w1 / w2);
}

PRIVATE long rmi(w1, w2)
	long w1, w2;
{
	if (w2 == 0) {
		if (!(IgnMask&BIT(EIDIVZ))) {
			trap(EIDIVZ);
		}
		else	return (0L);
	}
	return (w1 % w2);
}

PRIVATE long ngi(w1, nbytes)
	long w1;
	size nbytes;
{
	if (must_test && !(IgnMask&BIT(EIOVFL))) {
		if (w1 == i_mins(nbytes)) {
			trap(EIOVFL);
		}
	}
	return (-w1);
}

PRIVATE long sli(w1, w2, nbytes)	/* w1 << w2 */
	long w1, w2;
	size nbytes;
{
	if (must_test) {
#ifdef	LOGGING
		/* check shift distance */
		if (w2 < 0)	{
			warning(WSHNEG);
			w2 = 0;
		}
		if (w2 >= nbytes*8)	{
			warning(WSHLARGE);
			w2 = nbytes*8 - 1;
		}
#endif	LOGGING
	
		if (!(IgnMask&BIT(EIOVFL))) {
			/* check overflow */
			if (	(w1 >= 0 && (w1 >> (nbytes*8 - w2)) != 0)
			||	(w1 < 0 && (w1 >> (nbytes*8 - w2)) != -1)
			) {
				trap(EIOVFL);
			}
		}
	}	

	/* calculate result */
	return (w1 << w2);
}

/*ARGSUSED*/
PRIVATE long sri(w1, w2, nbytes)	/* w1 >> w2 */
	long w1, w2;
	size nbytes;
{
#ifdef	LOGGING
	if (must_test) {
		/* check shift distance */
		if (w2 < 0)	{
			warning(WSHNEG);
			w2 = 0;
		}
		if (w2 >= nbytes*8)	{
			warning(WSHLARGE);
			w2 = nbytes*8 - 1;
		}
	}
#endif	LOGGING
	
	/* calculate result */
	return (w1 >> w2);
}

