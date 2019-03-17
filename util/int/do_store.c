/** @file
 *  Sources of the "STORE" group instructions
 */

/* $Id$ */

#include	"em_abs.h"
#include	"global.h"
#include	"log.h"
#include	"mem.h"
#include	"trap.h"
#include	"text.h"
#include	"fra.h"
#include	"warn.h"

/** STL l: Store local or parameter */
void DoSTL(register long l)
{
	LOG(("@S6 DoSTL(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	popw_st(loc_addr(l));
}

/** STE g: Store external */
void DoSTE(register unsigned long arg)
{
	register ptr p = i2p(arg);

	LOG(("@S6 DoSTE(%lu)", p));
	spoilFRA();
	popw_m(arg_g(p));
}

/** SIL l: Store into word pointed to by l-th local or parameter */
void DoSIL(register long l)
{
	LOG(("@S6 DoSIL(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	popw_m(st_lddp(loc_addr(l)));
}

/** STF f: Store offsetted */
void DoSTF(register long l)
{
	register ptr p = dppop();

	LOG(("@S6 DoSTF(%ld)", l));
	spoilFRA();
	popw_m(p + arg_f(l));
}

/** STI o: Store indirect o bytes (pop address, then data) */
void DoSTI(register size l)
{
	register ptr p = dppop();

	LOG(("@S6 DoSTI(%ld)", l));
	spoilFRA();
	pop_m(p, arg_o(l));
}

/** STS w: Store indirect, w-byte integer on top of stack gives object size */
void DoSTS(register size l)
{
	register ptr p;

	LOG(("@S6 DoSTS(%ld)", l));
	spoilFRA();
	l = upop(arg_wi(l));
	p = dppop();
	pop_m(p, arg_o(l));
}

/** SDL l: Store double local or parameter */
void DoSDL(register long l)
{

	LOG(("@S6 DoSDL(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	pop_st(loc_addr(l), dwsize);
}

/** SDE g: Store double external */
void DoSDE(register unsigned long arg)
{
	register ptr p = i2p(arg);

	LOG(("@S6 DoSDE(%lu)", p));
	spoilFRA();
	pop_m(arg_g(p), dwsize);
}

/** SDF f: Store double offsetted */
void DoSDF(register long l)
{
	register ptr p = dppop();

	LOG(("@S6 DoSDF(%ld)", l));
	spoilFRA();
	pop_m(p + arg_f(l), dwsize);
}
