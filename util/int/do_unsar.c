/*
 * Sources of the "UNSIGNED ARITHMETIC" group instructions
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
#endif	LOGGING

#define	adu(w1,w2)	(unsigned long)(w1 + w2)
#define	sbu(w1,w2)	(unsigned long)(w1 - w2)
#define	mlu(w1,w2)	(unsigned long)(w1 * w2)

PRIVATE unsigned long dvu(), rmu(), slu(), sru();

DoADUl2(arg)
	size arg;
{
	/* ADU w: Addition */
	register size l = (L_arg_2() * arg);
	register unsigned long t = upop(arg_wi(l));

	LOG(("@U6 DoADUl2(%ld)", l));
	spoilFRA();
	npush((long) adu(upop(l), t), l);
}

DoADUz()
{
	/* ADU w: Addition */
	register size l = uwpop();
	register unsigned long t = upop(arg_wi(l));

	LOG(("@U6 DoADUz(%ld)", l));
	spoilFRA();
	npush((long) adu(upop(l), t), l);
}

DoSBUl2(arg)
	size arg;
{
	/* SBU w: Subtraction */
	register size l = (L_arg_2() * arg);
	register unsigned long t = upop(arg_wi(l));

	LOG(("@U6 DoSBUl2(%ld)", l));
	spoilFRA();
	npush((long) sbu(upop(l), t), l);
}

DoSBUz()
{
	/* SBU w: Subtraction */
	register size l = uwpop();
	register unsigned long t = upop(arg_wi(l));

	LOG(("@U6 DoSBUz(%ld)", l));
	spoilFRA();
	npush((long) sbu(upop(l), t), l);
}

DoMLUl2(arg)
	size arg;
{
	/* MLU w: Multiplication */
	register size l = (L_arg_2() * arg);
	register unsigned long t = upop(arg_wi(l));

	LOG(("@U6 DoMLUl2(%ld)", l));
	spoilFRA();
	npush((long) mlu(upop(l), t), l);
}

DoMLUz()
{
	/* MLU w: Multiplication */
	register size l = uwpop();
	register unsigned long t = upop(arg_wi(l));

	LOG(("@U6 DoMLUz(%ld)", l));
	spoilFRA();
	npush((long) mlu(upop(l), t), l);
}

DoDVUl2(arg)
	size arg;
{
	/* DVU w: Division */
	register size l = (L_arg_2() * arg);
	register unsigned long t = upop(arg_wi(l));

	LOG(("@U6 DoDVUl2(%ld)", l));
	spoilFRA();
	npush((long) dvu(upop(l), t), l);
}

DoDVUz()
{
	/* DVU w: Division */
	register size l = uwpop();
	register unsigned long t = upop(arg_wi(l));

	LOG(("@U6 DoDVUz(%ld)", l));
	spoilFRA();
	npush((long) dvu(upop(l), t), l);
}

DoRMUl2(arg)
	size arg;
{
	/* RMU w: Remainder */
	register size l = (L_arg_2() * arg);
	register unsigned long t = upop(arg_wi(l));

	LOG(("@U6 DoRMUl2(%ld)", l));
	spoilFRA();
	npush((long) rmu(upop(l), t), l);
}

DoRMUz()
{
	/* RMU w: Remainder */
	register size l = uwpop();
	register unsigned long t = upop(arg_wi(l));

	LOG(("@U6 DoRMUz(%ld)", l));
	spoilFRA();
	npush((long) rmu(upop(l), t), l);
}

DoSLUl2(arg)
	size arg;
{
	/* SLU w: Shift left */
	register size l = (L_arg_2() * arg);
	register unsigned long t = uwpop();

	LOG(("@U6 DoSLUl2(%ld)", l));
	spoilFRA();
	l = arg_wi(l);
	npush((long) slu(upop(l), t, l), l);
}

DoSLUz()
{
	/* SLU w: Shift left */
	register size l = uwpop();
	register unsigned long t = uwpop();

	LOG(("@U6 DoSLUz(%ld)", l));
	spoilFRA();
	l = arg_wi(l);
	npush((long) slu(upop(l), t, l), l);
}

DoSRUl2(arg)
	size arg;
{
	/* SRU w: Shift right */
	register size l = (L_arg_2() * arg);
	register unsigned long t = uwpop();

	LOG(("@U6 DoSRUl2(%ld)", l));
	spoilFRA();
	l = arg_wi(l);
	npush((long) sru(upop(l), t, l), l);
}

DoSRUz()
{
	/* SRU w: Shift right */
	register size l = uwpop();
	register unsigned long t = uwpop();

	LOG(("@U6 DoSRUz(%ld)", l));
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
#endif	LOGGING

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
#endif	LOGGING

	/* calculate result */
	return (w1 >> w2);
}

