/*
 * Sources of the "STORE" group instructions
 */

/* $Header$ */

#include	<em_abs.h>
#include	"global.h"
#include	"log.h"
#include	"mem.h"
#include	"trap.h"
#include	"text.h"
#include	"fra.h"
#include	"warn.h"

DoSTLm(arg)
	long arg;
{
	/* STL l: Store local or parameter */
	register long l = arg_l(arg);

	LOG(("@S6 DoSTLm(%ld)", l));
	spoilFRA();
	pop_st(loc_addr(l), wsize);
}

DoSTLn2(arg)
	long arg;
{
	/* STL l: Store local or parameter */
	register long l = (N_arg_2() * arg);

	LOG(("@S6 DoSTLn2(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	pop_st(loc_addr(l), wsize);
}

DoSTLn4(arg)
	long arg;
{
	/* STL l: Store local or parameter */
	register long l = (N_arg_4() * arg);

	LOG(("@S6 DoSTLn4(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	pop_st(loc_addr(l), wsize);
}

DoSTLp2(arg)
	long arg;
{
	/* STL l: Store local or parameter */
	register long l = (P_arg_2() * arg);

	LOG(("@S6 DoSTLp2(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	pop_st(loc_addr(l), wsize);
}

DoSTLp4(arg)
	long arg;
{
	/* STL l: Store local or parameter */
	register long l = (P_arg_4() * arg);

	LOG(("@S6 DoSTLp4(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	pop_st(loc_addr(l), wsize);
}

DoSTLs(hob, wfac)
	long hob;
	size wfac;
{
	/* STL l: Store local or parameter */
	register long l = (S_arg(hob) * wfac);

	LOG(("@S6 DoSTLs(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	pop_st(loc_addr(l), wsize);
}

DoSTEl2(arg)
	long arg;
{
	/* STE g: Store external */
	register ptr p = i2p(L_arg_2() * arg);

	LOG(("@S6 DoSTEl2(%lu)", p));
	spoilFRA();
	pop_m(arg_g(p), wsize);
}

DoSTEl4(arg)
	long arg;
{
	/* STE g: Store external */
	register ptr p = i2p(L_arg_4() * arg);

	LOG(("@S6 DoSTEl4(%lu)", p));
	spoilFRA();
	pop_m(arg_g(p), wsize);
}

DoSTEs(hob, wfac)
	long hob;
	size wfac;
{
	/* STE g: Store external */
	register ptr p = i2p(S_arg(hob) * wfac);

	LOG(("@S6 DoSTEs(%lu)", p));
	spoilFRA();
	pop_m(arg_g(p), wsize);
}

DoSILn2(arg)
	long arg;
{
	/* SIL l: Store into word pointed to by l-th local or parameter */
	register long l = (N_arg_2() * arg);

	LOG(("@S6 DoSILn2(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	pop_m(st_lddp(loc_addr(l)), wsize);
}

DoSILn4(arg)
	long arg;
{
	/* SIL l: Store into word pointed to by l-th local or parameter */
	register long l = (N_arg_4() * arg);

	LOG(("@S6 DoSILn4(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	pop_m(st_lddp(loc_addr(l)), wsize);
}

DoSILp2(arg)
	long arg;
{
	/* SIL l: Store into word pointed to by l-th local or parameter */
	register long l = (P_arg_2() * arg);

	LOG(("@S6 DoSILp2(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	pop_m(st_lddp(loc_addr(l)), wsize);
}

DoSILp4(arg)
	long arg;
{
	/* SIL l: Store into word pointed to by l-th local or parameter */
	register long l = (P_arg_4() * arg);

	LOG(("@S6 DoSILp4(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	pop_m(st_lddp(loc_addr(l)), wsize);
}

DoSILs(hob, wfac)
	long hob;
	size wfac;
{
	/* SIL l: Store into word pointed to by l-th local or parameter */
	register long l = (S_arg(hob) * wfac);

	LOG(("@S6 DoSILs(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	pop_m(st_lddp(loc_addr(l)), wsize);
}

DoSTFl2(arg)
	long arg;
{
	/* STF f: Store offsetted */
	register long l = (L_arg_2() * arg);
	register ptr p = dppop();

	LOG(("@S6 DoSTFl2(%ld)", l));
	spoilFRA();
	pop_m(p + arg_f(l), wsize);
}

DoSTFl4(arg)
	long arg;
{
	/* STF f: Store offsetted */
	register long l = (L_arg_4() * arg);
	register ptr p = dppop();

	LOG(("@S6 DoSTFl4(%ld)", l));
	spoilFRA();
	pop_m(p + arg_f(l), wsize);
}

DoSTFm(arg)
	long arg;
{
	/* STF f: Store offsetted */
	register long l = arg;
	register ptr p = dppop();

	LOG(("@S6 DoSTFm(%ld)", l));
	spoilFRA();
	pop_m(p + arg_f(l), wsize);
}

DoSTFs(hob, wfac)
	long hob;
	size wfac;
{
	/* STF f: Store offsetted */
	register long l = (S_arg(hob) * wfac);
	register ptr p = dppop();

	LOG(("@S6 DoSTFs(%ld)", l));
	spoilFRA();
	pop_m(p + arg_f(l), wsize);
}

DoSTIl2(arg)
	size arg;
{
	/* STI o: Store indirect o bytes (pop address, then data) */
	register size l = (L_arg_2() * arg);
	register ptr p = dppop();

	LOG(("@S6 DoSTIl2(%ld)", l));
	spoilFRA();
	pop_m(p, arg_o(l));
}

DoSTIl4(arg)
	size arg;
{
	/* STI o: Store indirect o bytes (pop address, then data) */
	register size l = (L_arg_4() * arg);
	register ptr p = dppop();

	LOG(("@S6 DoSTIl4(%ld)", l));
	spoilFRA();
	pop_m(p, arg_o(l));
}

DoSTIm(arg)
	size arg;
{
	/* STI o: Store indirect o bytes (pop address, then data) */
	register ptr p = dppop();

	LOG(("@S6 DoSTIm(%ld)", arg));
	spoilFRA();
	pop_m(p, arg_o(arg));
}

DoSTIs(hob, wfac)
	long hob;
	size wfac;
{
	/* STI o: Store indirect o bytes (pop address, then data) */
	register size l = (S_arg(hob) * wfac);
	register ptr p = dppop();

	LOG(("@S6 DoSTIs(%ld)", l));
	spoilFRA();
	pop_m(p, arg_o(l));
}

DoSTSl2(arg)
	size arg;
{
	/* STS w: Store indirect, w-byte integer on top of stack gives object size */
	register size l = (P_arg_2() * arg);
	register ptr p;

	LOG(("@S6 DoSTSl2(%ld)", l));
	spoilFRA();
	l = upop(arg_wi(l));
	p = dppop();
	pop_m(p, arg_o(l));
}

DoSTSz()				/* the arg 'w' is on top of stack */
{
	/* STS w: Store indirect, w-byte integer on top of stack gives object size */
	register size l = upop(wsize);
	register ptr p;

	LOG(("@S6 DoSTSz(%ld)", l));
	spoilFRA();
	l = upop(arg_wi(l));
	p = dppop();
	pop_m(p, arg_o(l));
}

DoSDLn2(arg)
	long arg;
{
	/* SDL l: Store double local or parameter */
	register long l = (N_arg_2() * arg);

	LOG(("@S6 DoSDLn2(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	pop_st(loc_addr(l), dwsize);
}

DoSDLn4(arg)
	long arg;
{
	/* SDL l: Store double local or parameter */
	register long l = (N_arg_4() * arg);

	LOG(("@S6 DoSDLn4(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	pop_st(loc_addr(l), dwsize);
}

DoSDLp2(arg)
	long arg;
{
	/* SDL l: Store double local or parameter */
	register long l = (P_arg_2() * arg);

	LOG(("@S6 DoSDLp2(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	pop_st(loc_addr(l), dwsize);
}

DoSDLp4(arg)
	long arg;
{
	/* SDL l: Store double local or parameter */
	register long l = (P_arg_4() * arg);

	LOG(("@S6 DoSDLp4(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	pop_st(loc_addr(l), dwsize);
}

DoSDLs(hob, wfac)
	long hob;
	size wfac;
{
	/* SDL l: Store double local or parameter */
	register long l = (S_arg(hob) * wfac);

	LOG(("@S6 DoSDLs(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	pop_st(loc_addr(l), dwsize);
}

DoSDEu(arg)
	long arg;
{
	/* SDE g: Store double external */
	register ptr p = i2p(U_arg() * arg);

	LOG(("@S6 DoSDEu(%lu)", p));
	spoilFRA();
	pop_m(arg_g(p), dwsize);
}

DoSDEl4(arg)
	long arg;
{
	/* SDE g: Store double external */
	register ptr p = i2p(L_arg_4() * arg);

	LOG(("@S6 DoSDEl4(%lu)", p));
	spoilFRA();
	pop_m(arg_g(p), dwsize);
}

DoSDFl2(arg)
	long arg;
{
	/* SDF f: Store double offsetted */
	register long l = (L_arg_2() * arg);
	register ptr p = dppop();

	LOG(("@S6 DoSDFl2(%ld)", l));
	spoilFRA();
	pop_m(p + arg_f(l), dwsize);
}

DoSDFl4(arg)
	long arg;
{
	/* SDF f: Store double offsetted */
	register long l = (L_arg_4() * arg);
	register ptr p = dppop();

	LOG(("@S6 DoSDFl4(%ld)", l));
	spoilFRA();
	pop_m(p + arg_f(l), dwsize);
}
