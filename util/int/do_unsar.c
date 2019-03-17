/** @file
 * Sources of the "UNSIGNED ARITHMETIC" group instructions
 */

/* $Id$ */

#include	"em_abs.h"
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

/** ADU w: Addition */
void DoADU(register size l)
{
	register unsigned long t = upop(arg_wi(l));

	LOG(("@U6 DoADU(%ld)", l));
	spoilFRA();
	npush((long) adu(upop(l), t), l);
}

/** SBU w: Subtraction */
void DoSBU(register size l)
{
	register unsigned long t = upop(arg_wi(l));

	LOG(("@U6 DoSBU(%ld)", l));
	spoilFRA();
	npush((long) sbu(upop(l), t), l);
}

/** MLU w: Multiplication */
void DoMLU(register size l)
{
	register unsigned long t = upop(arg_wi(l));

	LOG(("@U6 DoMLU(%ld)", l));
	spoilFRA();
	npush((long) mlu(upop(l), t), l);
}

/** DVU w: Division */
void DoDVU(register size l)
{
	register unsigned long t = upop(arg_wi(l));

	LOG(("@U6 DoDVU(%ld)", l));
	spoilFRA();
	npush((long) dvu(upop(l), t), l);
}

/** RMU w: Remainder */
void DoRMU(register size l)
{
	register unsigned long t = upop(arg_wi(l));

	LOG(("@U6 DoRMU(%ld)", l));
	spoilFRA();
	npush((long) rmu(upop(l), t), l);
}

/** SLU w: Shift left */
void DoSLU(register size l)
{
	register unsigned long t = uwpop();

	LOG(("@U6 DoSLU(%ld)", l));
	spoilFRA();
	l = arg_wi(l);
	npush((long) slu(upop(l), t, l), l);
}

/** SRU w: Shift right */
void DoSRU(register size l)
{
	register unsigned long t = uwpop();

	LOG(("@U6 DoSRU(%ld)", l));
	spoilFRA();
	l = arg_wi(l);
	npush((long) sru(upop(l), t, l), l);
}

PRIVATE unsigned long dvu(
	unsigned long w1,
	unsigned long w2)
{
	if (w2 == 0) {
		if (!(IgnMask&BIT(EIDIVZ))) {
			trap(EIDIVZ);
		}
		else	return (0L);
	}
	return (w1 / w2);
}

PRIVATE unsigned long rmu(
	unsigned long w1,
	unsigned long w2)
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
PRIVATE unsigned long slu(
		unsigned long w1,
		unsigned long w2,
		size nbytes)
{
		/* w1 << w2 */
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
PRIVATE unsigned long sru(
	unsigned long w1,
	unsigned long w2,
	size nbytes)
{
	/* w1 >> w2 */
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

