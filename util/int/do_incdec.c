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

DoINCz()
{
	/* INC -: Increment word on top of stack by 1 (*) */
	LOG(("@Z6 DoINCz()"));
	spoilFRA();
	wpush(inc(swpop()));
}

DoINLm(arg)
	long arg;
{
	/* INL l: Increment local or parameter (*) */
	register long l = arg_l(arg);
	register ptr p;

	LOG(("@Z6 DoINLm(%ld)", l));
	spoilFRA();
	p = loc_addr(l);
	st_stw(p, inc(st_ldsw(p)));
}

DoINLn2(arg)
	long arg;
{
	/* INL l: Increment local or parameter (*) */
	register long l = (N_arg_2() * arg);
	register ptr p;

	LOG(("@Z6 DoINLn2(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	p = loc_addr(l);
	st_stw(p, inc(st_ldsw(p)));
}

DoINLn4(arg)
	long arg;
{
	/* INL l: Increment local or parameter (*) */
	register long l = (N_arg_4() * arg);
	register ptr p;

	LOG(("@Z6 DoINLn4(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	p = loc_addr(l);
	st_stw(p, inc(st_ldsw(p)));
}

DoINLp2(arg)
	long arg;
{
	/* INL l: Increment local or parameter (*) */
	register long l = (P_arg_2() * arg);
	register ptr p;

	LOG(("@Z6 DoINLp2(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	p = loc_addr(l);
	st_stw(p, inc(st_ldsw(p)));
}

DoINLp4(arg)
	long arg;
{
	/* INL l: Increment local or parameter (*) */
	register long l = (P_arg_4() * arg);
	register ptr p;

	LOG(("@Z6 DoINLp4(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	p = loc_addr(l);
	st_stw(p, inc(st_ldsw(p)));
}

DoINLs(hob, wfac)
	long hob;
	size wfac;
{
	/* INL l: Increment local or parameter (*) */
	register long l = (S_arg(hob) * wfac);
	register ptr p;

	LOG(("@Z6 DoINLs(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	p = loc_addr(l);
	st_stw(p, inc(st_ldsw(p)));
}

DoINEl2(arg)
	long arg;
{
	/* INE g: Increment external (*) */
	register ptr p = i2p(L_arg_2() * arg);

	LOG(("@Z6 DoINEl2(%lu)", p));
	spoilFRA();
	p = arg_g(p);
	dt_stw(p, inc(dt_ldsw(p)));
}

DoINEl4(arg)
	long arg;
{
	/* INE g: Increment external (*) */
	register ptr p = i2p(L_arg_4() * arg);

	LOG(("@Z6 DoINEl4(%lu)", p));
	spoilFRA();
	p = arg_g(p);
	dt_stw(p, inc(dt_ldsw(p)));
}

DoINEs(hob, wfac)
	long hob;
	size wfac;
{
	/* INE g: Increment external (*) */
	register ptr p = i2p(S_arg(hob) * wfac);

	LOG(("@Z6 DoINEs(%lu)", p));
	spoilFRA();
	p = arg_g(p);
	dt_stw(p, inc(dt_ldsw(p)));
}

DoDECz()
{
	/* DEC -: Decrement word on top of stack by 1 (*) */
	LOG(("@Z6 DoDECz()"));
	spoilFRA();
	wpush(dec(swpop()));
}

DoDELn2(arg)
	long arg;
{
	/* DEL l: Decrement local or parameter (*) */
	register long l = (N_arg_2() * arg);
	register ptr p;

	LOG(("@Z6 DoDELn2(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	p = loc_addr(l);
	st_stw(p, dec(st_ldsw(p)));
}

DoDELn4(arg)
	long arg;
{
	/* DEL l: Decrement local or parameter (*) */
	register long l = (N_arg_4() * arg);
	register ptr p;

	LOG(("@Z6 DoDELn4(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	p = loc_addr(l);
	st_stw(p, dec(st_ldsw(p)));
}

DoDELp2(arg)
	long arg;
{
	/* DEL l: Decrement local or parameter (*) */
	register long l = (P_arg_2() * arg);
	register ptr p;

	LOG(("@Z6 DoDELp2(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	p = loc_addr(l);
	st_stw(p, dec(st_ldsw(p)));
}

DoDELp4(arg)
	long arg;
{
	/* DEL l: Decrement local or parameter (*) */
	register long l = (P_arg_4() * arg);
	register ptr p;

	LOG(("@Z6 DoDELp4(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	p = loc_addr(l);
	st_stw(p, dec(st_ldsw(p)));
}

DoDELs(hob, wfac)
	long hob;
	size wfac;
{
	/* DEL l: Decrement local or parameter (*) */
	register long l = (S_arg(hob) * wfac);
	register ptr p;

	LOG(("@Z6 DoDELs(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	p = loc_addr(l);
	st_stw(p, dec(st_ldsw(p)));
}

DoDEEl2(arg)
	long arg;
{
	/* DEE g: Decrement external (*) */
	register ptr p = i2p(L_arg_2() * arg);

	LOG(("@Z6 DoDEEl2(%lu)", p));
	spoilFRA();
	p = arg_g(p);
	dt_stw(p, dec(dt_ldsw(p)));
}

DoDEEl4(arg)
	long arg;
{
	/* DEE g: Decrement external (*) */
	register ptr p = i2p(L_arg_4() * arg);

	LOG(("@Z6 DoDEEl4(%lu)", p));
	spoilFRA();
	p = arg_g(p);
	dt_stw(p, dec(dt_ldsw(p)));
}

DoDEEs(hob, wfac)
	long hob;
	size wfac;
{
	/* DEE g: Decrement external (*) */
	register ptr p = i2p(S_arg(hob) * wfac);

	LOG(("@Z6 DoDEEs(%lu)", p));
	spoilFRA();
	p = arg_g(p);
	dt_stw(p, dec(dt_ldsw(p)));
}

DoZRLm(arg)
	long arg;
{
	/* ZRL l: Zero local or parameter */
	register long l = arg_l(arg);

	LOG(("@Z6 DoZRLm(%ld)", l));
	spoilFRA();
	st_stw(loc_addr(l), 0L);
}

DoZRLn2(arg)
	long arg;
{
	/* ZRL l: Zero local or parameter */
	register long l = (N_arg_2() * arg);

	LOG(("@Z6 DoZRLn2(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	st_stw(loc_addr(l), 0L);
}

DoZRLn4(arg)
	long arg;
{
	/* ZRL l: Zero local or parameter */
	register long l = (N_arg_4() * arg);

	LOG(("@Z6 DoZRLn4(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	st_stw(loc_addr(l), 0L);
}

DoZRLp2(arg)
	long arg;
{
	/* ZRL l: Zero local or parameter */
	register long l = (P_arg_2() * arg);

	LOG(("@Z6 DoZRLp2(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	st_stw(loc_addr(l), 0L);
}

DoZRLp4(arg)
	long arg;
{
	/* ZRL l: Zero local or parameter */
	register long l = (P_arg_4() * arg);

	LOG(("@Z6 DoZRLp4(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	st_stw(loc_addr(l), 0L);
}

DoZRLs(hob, wfac)
	long hob;
	size wfac;
{
	/* ZRL l: Zero local or parameter */
	register long l = (S_arg(hob) * wfac);

	LOG(("@Z6 DoZRLs(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	st_stw(loc_addr(l), 0L);
}

DoZREl2(arg)
	long arg;
{
	/* ZRE g: Zero external */
	register ptr p = i2p(L_arg_2() * arg);

	LOG(("@Z6 DoZREl2(%lu)", p));
	spoilFRA();
	dt_stw(arg_g(p), 0L);
}

DoZREl4(arg)
	long arg;
{
	/* ZRE g: Zero external */
	register ptr p = i2p(L_arg_4() * arg);

	LOG(("@Z6 DoZREl4(%lu)", p));
	spoilFRA();
	dt_stw(arg_g(p), 0L);
}

DoZREs(hob, wfac)
	long hob;
	size wfac;
{
	/* ZRE g: Zero external */
	register ptr p = i2p(S_arg(hob) * wfac);

	LOG(("@Z6 DoZREs(%lu)", p));
	spoilFRA();
	dt_stw(arg_g(p), 0L);
}

DoZRFl2(arg)
	size arg;
{
	/* ZRF w: Load a floating zero of size w */
#ifndef	NOFLOAT
	register size l = (L_arg_2() * arg);

	LOG(("@Z6 DoZRFl2(%ld)", l));
	spoilFRA();
	fpush(0.0, arg_wf(l));
#else	NOFLOAT
	arg = arg;
	nofloat();
#endif	NOFLOAT
}

DoZRFz()
{
	/* ZRF w: Load a floating zero of size w */
#ifndef	NOFLOAT
	register size l = uwpop();

	LOG(("@Z6 DoZRFz(%ld)", l));
	spoilFRA();
	fpush(0.0, arg_wf(l));
#else	NOFLOAT
	nofloat();
#endif	NOFLOAT
}

DoZERl2(arg)
	size arg;
{
	/* ZER w: Load w zero bytes */
	register size i, l = (L_arg_2() * arg);

	LOG(("@Z6 DoZERl2(%ld)", l));
	spoilFRA();
	for (i = arg_w(l); i; i -= wsize)
		wpush(0L);
}

DoZERs(hob, wfac)
	long hob;
	size wfac;
{
	/* ZER w: Load w zero bytes */
	register size i, l = (S_arg(hob) * wfac);

	LOG(("@Z6 DoZERs(%ld)", l));
	spoilFRA();
	for (i = arg_w(l); i; i -= wsize)
		wpush(0L);
}

DoZERz()
{
	/* ZER w: Load w zero bytes */
	register size i, l = swpop();

	LOG(("@Z6 DoZERz(%ld)", l));
	spoilFRA();
	for (i = arg_w(l); i; i -= wsize)
		wpush(0L);
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

