/*
 * Sources of the "INTEGER ARITHMETIC" group instructions
 */

/* $Id$ */

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

DoADI(l)
	register size l;
{
	/* ADI w: Addition (*) */
	register long t = spop(arg_wi(l));

	LOG(("@I6 DoADI(%ld)", l));
	spoilFRA();
	npush(adi(spop(l), t, l), l);
}

DoSBI(l)
	register size l;
{
	/* SBI w: Subtraction (*) */
	register long t = spop(arg_wi(l));

	LOG(("@I6 DoSBI(%ld)", l));
	spoilFRA();
	npush(sbi(spop(l), t, l), l);
}

DoMLI(l)
	register size l;
{
	/* MLI w: Multiplication (*) */
	register long t = spop(arg_wi(l));

	LOG(("@I6 DoMLI(%ld)", l));
	spoilFRA();
	npush(mli(spop(l), t, l), l);
}

DoDVI(l)
	register size l;
{
	/* DVI w: Division (*) */
	register long t = spop(arg_wi(l));

	LOG(("@I6 DoDVI(%ld)", l));
	spoilFRA();
	npush(dvi(spop(l), t), l);
}

DoRMI(l)
	register size l;
{
	/* RMI w: Remainder (*) */
	register long t = spop(arg_wi(l));

	LOG(("@I6 DoRMI(%ld)", l));
	spoilFRA();
	npush(rmi(spop(l), t), l);
}

DoNGI(l)
	register size l;
{
	/* NGI w: Negate (two's complement) (*) */

	LOG(("@I6 DoNGI(%ld)", l));
	spoilFRA();
	l = arg_wi(l);
	npush(ngi(spop(l), l), l);
}

DoSLI(l)
	register size l;
{
	/* SLI w: Shift left (*) */
	register long t = swpop();

	LOG(("@I6 DoSLI(%ld)", l));
	spoilFRA();
	l = arg_wi(l);
	npush(sli(spop(l), t, l), l);
}

DoSRI(l)
	register size l;
{
	/* SRI w: Shift right (*) */
	register long t = swpop();

	LOG(("@I6 DoSRI(%ld)", l));
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
#endif	/* LOGGING */
	
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
#endif	/* LOGGING */
	
	/* calculate result */
	return (w1 >> w2);
}

