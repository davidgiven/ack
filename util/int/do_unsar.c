/*
 * Sources of the "UNSIGNED ARITHMETIC" group instructions
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

/************************************************************************
 *	No checking is performed, except for division by zero.		*
 *	The operands popped from the stack are put in unsigned		*
 *	longs. Now the required operation can be performed		*
 *	immediately. Whether the wordsize is two or four bytes		*
 *	doesn't matter. Alas, arithmetic is performed modulo		*
 *	the highest unsigned number for the given size plus 1.		*
 ************************************************************************/

#ifdef	LOGGING
extern int must_test;
#endif	/* LOGGING */

#define	adu(w1,w2)	(unsigned long)(w1 + w2)
#define	sbu(w1,w2)	(unsigned long)(w1 - w2)
#define	mlu(w1,w2)	(unsigned long)(w1 * w2)

PRIVATE unsigned long dvu(), rmu(), slu(), sru();

DoADU(l)
	register size l;
{
	/* ADU w: Addition */
	register unsigned long t = upop(arg_wi(l));

	LOG(("@U6 DoADU(%ld)", l));
	spoilFRA();
	npush((long) adu(upop(l), t), l);
}

DoSBU(l)
	register size l;
{
	/* SBU w: Subtraction */
	register unsigned long t = upop(arg_wi(l));

	LOG(("@U6 DoSBU(%ld)", l));
	spoilFRA();
	npush((long) sbu(upop(l), t), l);
}

DoMLU(l)
	register size l;
{
	/* MLU w: Multiplication */
	register unsigned long t = upop(arg_wi(l));

	LOG(("@U6 DoMLU(%ld)", l));
	spoilFRA();
	npush((long) mlu(upop(l), t), l);
}

DoDVU(l)
	register size l;
{
	/* DVU w: Division */
	register unsigned long t = upop(arg_wi(l));

	LOG(("@U6 DoDVU(%ld)", l));
	spoilFRA();
	npush((long) dvu(upop(l), t), l);
}

DoRMU(l)
	register size l;
{
	/* RMU w: Remainder */
	register unsigned long t = upop(arg_wi(l));

	LOG(("@U6 DoRMU(%ld)", l));
	spoilFRA();
	npush((long) rmu(upop(l), t), l);
}

DoSLU(l)
	register size l;
{
	/* SLU w: Shift left */
	register unsigned long t = uwpop();

	LOG(("@U6 DoSLU(%ld)", l));
	spoilFRA();
	l = arg_wi(l);
	npush((long) slu(upop(l), t, l), l);
}

DoSRU(l)
	register size l;
{
	/* SRU w: Shift right */
	register unsigned long t = uwpop();

	LOG(("@U6 DoSRU(%ld)", l));
	spoilFRA();
	l = arg_wi(l);
	npush((long) sru(upop(l), t, l), l);
}

PRIVATE unsigned long dvu(w1, w2)
	unsigned long w1, w2;
{
	if (w2 == 0) {
		if (!(IgnMask&BIT(EIDIVZ))) {
			trap(EIDIVZ);
		}
		else	return (0L);
	}
	return (w1 / w2);
}

PRIVATE unsigned long rmu(w1, w2)
	unsigned long w1, w2;
{
	if (w2 == 0) {
		if (!(IgnMask&BIT(EIDIVZ))) {
			trap(EIDIVZ);
		}
		else	return (0L);
	}
	return (w1 % w2);
}

/*ARGSUSED*/
PRIVATE unsigned long slu(w1, w2, nbytes)	/* w1 << w2 */
	unsigned long w1, w2;
	size nbytes;
{
#ifdef	LOGGING
	if (must_test) {
		/* check shift distance */
		if (w2 >= nbytes*8)	{
			warning(WSHLARGE);
			w2 = nbytes*8 - 1;
		}
	}
#endif	/* LOGGING */

	/* calculate result */
	return (w1 << w2);
}

/*ARGSUSED*/
PRIVATE unsigned long sru(w1, w2, nbytes)	/* w1 >> w2 */
	unsigned long w1, w2;
	size nbytes;
{
#ifdef	LOGGING
	if (must_test) {
		/* check shift distance */
		if (w2 >= nbytes*8)	{
			warning(WSHLARGE);
			w2 = nbytes*8 - 1;
		}
	}
#endif	/* LOGGING */

	/* calculate result */
	return (w1 >> w2);
}

