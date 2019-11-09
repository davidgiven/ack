/** @file
 *  Sources of the "INCREMENT/DECREMENT/ZERO" group instructions
 */

/* $Id$ */

#include	"em_abs.h"
#include	"global.h"
#include	"log.h"
#include	"nofloat.h"
#include	"trap.h"
#include	"mem.h"
#include	"data.h"
#include	"text.h"
#include	"stack.h"
#include	"fra.h"
#include	"warn.h"
#include	"switch.h"

PRIVATE long inc(long), dec(long);

/** INC -: Increment word on top of stack by 1 (*) */
void DoINC(void)
{
	LOG(("@Z6 DoINC()"));
	spoilFRA();
	wpush(inc(swpop()));
}

/** INL l: Increment local or parameter (*) */
void DoINL(register long l)
{
	register ptr p;

	LOG(("@Z6 DoINL(%ld)", l));
	spoilFRA();
	p = loc_addr(arg_l(l));
	st_stw(p, inc(st_ldsw(p)));
}

/** INE g: Increment external (*) */
void DoINE(register long arg)
{

	register ptr p = i2p(arg);

	LOG(("@Z6 DoINE(%lu)", p));
	spoilFRA();
	p = arg_g(p);
	dt_stw(p, inc(dt_ldsw(p)));
}

/** DEC -: Decrement word on top of stack by 1 (*) */
void DoDEC(void)
{

	LOG(("@Z6 DoDEC()"));
	spoilFRA();
	wpush(dec(swpop()));
}

/** DEL l: Decrement local or parameter (*) */
void DoDEL(register long l)
{

	register ptr p;

	LOG(("@Z6 DoDEL(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	p = loc_addr(l);
	st_stw(p, dec(st_ldsw(p)));
}

/** DEE g: Decrement external (*) */
void DoDEE(register long arg)
{

	register ptr p = i2p(arg);

	LOG(("@Z6 DoDEE(%lu)", p));
	spoilFRA();
	p = arg_g(p);
	dt_stw(p, dec(dt_ldsw(p)));
}

/** ZRL l: Zero local or parameter */
void DoZRL(register long l)
{


	LOG(("@Z6 DoZRL(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	st_stw(loc_addr(l), 0L);
}

/** ZRE g: Zero external */
void DoZRE(register long arg)
{

	register ptr p = i2p(arg);

	LOG(("@Z6 DoZRE(%lu)", p));
	spoilFRA();
	dt_stw(arg_g(p), 0L);
}

/** ZRF w: Load a floating zero of size w */
void DoZRF(register size l)
{

#ifndef	NOFLOAT
	LOG(("@Z6 DoZRF(%ld)", l));
	spoilFRA();
	fpush(0.0, arg_wf(l));
#else	/* NOFLOAT */
	arg = arg;
	nofloat();
#endif	/* NOFLOAT */
}
/** ZER w: Load w zero bytes */
void DoZER(register size l)
{
	LOG(("@Z6 DoZER(%ld)", l));
	spoilFRA();
	npush(0L, arg_w(l));

/*
	for (i = arg_w(l); i; i -= wsize)
		wpush(0L);
*/
}

PRIVATE long inc(long l)
{
	if (must_test && !(IgnMask&BIT(EIOVFL))) {
		if (l == i_maxsw)
			trap(EIOVFL);
	}
	return (l + 1);
}

PRIVATE long dec(long l)
{
	if (must_test && !(IgnMask&BIT(EIOVFL))) {
		if (l == i_minsw)
			trap(EIOVFL);
	}
	return (l - 1);
}

