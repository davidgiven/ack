/*
 * Sources of the "STORE" group instructions
 */

/* $Id$ */

#include	<em_abs.h>
#include	"global.h"
#include	"log.h"
#include	"mem.h"
#include	"trap.h"
#include	"text.h"
#include	"fra.h"
#include	"warn.h"

DoSTL(l)
	register long l;
{
	/* STL l: Store local or parameter */

	LOG(("@S6 DoSTL(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	popw_st(loc_addr(l));
}

DoSTE(arg)
	register unsigned long arg;
{
	/* STE g: Store external */
	register ptr p = i2p(arg);

	LOG(("@S6 DoSTE(%lu)", p));
	spoilFRA();
	popw_m(arg_g(p));
}

DoSIL(l)
	register long l;
{
	/* SIL l: Store into word pointed to by l-th local or parameter */

	LOG(("@S6 DoSIL(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	popw_m(st_lddp(loc_addr(l)));
}

DoSTF(l)
	register long l;
{
	/* STF f: Store offsetted */
	register ptr p = dppop();

	LOG(("@S6 DoSTF(%ld)", l));
	spoilFRA();
	popw_m(p + arg_f(l));
}

DoSTI(l)
	register size l;
{
	/* STI o: Store indirect o bytes (pop address, then data) */
	register ptr p = dppop();

	LOG(("@S6 DoSTI(%ld)", l));
	spoilFRA();
	pop_m(p, arg_o(l));
}

DoSTS(l)
	register size l;
{
	/* STS w: Store indirect, w-byte integer on top of stack gives object size */
	register ptr p;

	LOG(("@S6 DoSTS(%ld)", l));
	spoilFRA();
	l = upop(arg_wi(l));
	p = dppop();
	pop_m(p, arg_o(l));
}

DoSDL(l)
	register long l;
{
	/* SDL l: Store double local or parameter */

	LOG(("@S6 DoSDL(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	pop_st(loc_addr(l), dwsize);
}

DoSDE(arg)
	register unsigned long arg;
{
	/* SDE g: Store double external */
	register ptr p = i2p(arg);

	LOG(("@S6 DoSDE(%lu)", p));
	spoilFRA();
	pop_m(arg_g(p), dwsize);
}

DoSDF(l)
	register long l;
{
	/* SDF f: Store double offsetted */
	register ptr p = dppop();

	LOG(("@S6 DoSDF(%ld)", l));
	spoilFRA();
	pop_m(p + arg_f(l), dwsize);
}
