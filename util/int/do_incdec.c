/*
 * Sources of the "INCREMENT/DECREMENT/ZERO" group instructions
 */

/* $Header$ */

#include	<em_abs.h>
#include	"global.h"
#include	"log.h"
#include	"nofloat.h"
#include	"trap.h"
#include	"mem.h"
#include	"text.h"
#include	"fra.h"
#include	"warn.h"

PRIVATE long inc(), dec();

DoINC()
{
	/* INC -: Increment word on top of stack by 1 (*) */
	LOG(("@Z6 DoINC()"));
	spoilFRA();
	wpush(inc(swpop()));
}

DoINL(l)
	register long l;
{
	/* INL l: Increment local or parameter (*) */
	register ptr p;

	LOG(("@Z6 DoINL(%ld)", l));
	spoilFRA();
	p = loc_addr(arg_l(l));
	st_stw(p, inc(st_ldsw(p)));
}

DoINE(arg)
	register long arg;
{
	/* INE g: Increment external (*) */
	register ptr p = i2p(arg);

	LOG(("@Z6 DoINE(%lu)", p));
	spoilFRA();
	p = arg_g(p);
	dt_stw(p, inc(dt_ldsw(p)));
}

DoDEC()
{
	/* DEC -: Decrement word on top of stack by 1 (*) */
	LOG(("@Z6 DoDEC()"));
	spoilFRA();
	wpush(dec(swpop()));
}

DoDEL(l)
	register long l;
{
	/* DEL l: Decrement local or parameter (*) */
	register ptr p;

	LOG(("@Z6 DoDEL(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	p = loc_addr(l);
	st_stw(p, dec(st_ldsw(p)));
}

DoDEE(arg)
	register long arg;
{
	/* DEE g: Decrement external (*) */
	register ptr p = i2p(arg);

	LOG(("@Z6 DoDEE(%lu)", p));
	spoilFRA();
	p = arg_g(p);
	dt_stw(p, dec(dt_ldsw(p)));
}

DoZRL(l)
	register long l;
{
	/* ZRL l: Zero local or parameter */

	LOG(("@Z6 DoZRL(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	st_stw(loc_addr(l), 0L);
}

DoZRE(arg)
	register long arg;
{
	/* ZRE g: Zero external */
	register ptr p = i2p(arg);

	LOG(("@Z6 DoZRE(%lu)", p));
	spoilFRA();
	dt_stw(arg_g(p), 0L);
}

DoZRF(l)
	register size l;
{
	/* ZRF w: Load a floating zero of size w */
#ifndef	NOFLOAT
	LOG(("@Z6 DoZRF(%ld)", l));
	spoilFRA();
	fpush(0.0, arg_wf(l));
#else	/* NOFLOAT */
	arg = arg;
	nofloat();
#endif	/* NOFLOAT */
}

DoZER(l)
	register size l;
{
	/* ZER w: Load w zero bytes */
	register size i;

	LOG(("@Z6 DoZER(%ld)", l));
	spoilFRA();
	npush(0L, arg_w(l));

/*
	for (i = arg_w(l); i; i -= wsize)
		wpush(0L);
*/
}

PRIVATE long inc(l)
	long l;
{
	if (must_test && !(IgnMask&BIT(EIOVFL))) {
		if (l == i_maxsw)
			trap(EIOVFL);
	}
	return (l + 1);
}

PRIVATE long dec(l)
	long l;
{
	if (must_test && !(IgnMask&BIT(EIOVFL))) {
		if (l == i_minsw)
			trap(EIOVFL);
	}
	return (l - 1);
}

