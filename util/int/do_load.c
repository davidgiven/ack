/*
 * Sources of the "LOAD" group instructions
 */

/* $Id$ */

#include	<em_abs.h>
#include	"global.h"
#include	"log.h"
#include	"mem.h"
#include	"trap.h"
#include	"text.h"
#include	"fra.h"
#include	"rsb.h"
#include	"warn.h"

PRIVATE ptr lexback_LB();

DoLOC(l)
	register long l;
{
	/* LOC c: Load constant (i.e. push one word onto the stack) */

	LOG(("@L6 DoLOC(%ld)", l));
	spoilFRA();
	wpush(arg_c(l));
}

DoLDC(l)
	register long l;
{
	/* LDC d: Load double constant ( push two words ) */

	LOG(("@L6 DoLDC(%ld)", l));
	spoilFRA();
	l = arg_d(l);
	npush(l, dwsize);
}

DoLOL(l)
	register long l;
{
	/* LOL l: Load word at l-th local (l<0) or parameter (l>=0) */

	LOG(("@L6 DoLOL(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	pushw_st(loc_addr(l));
}

DoLOE(arg)
	register long arg;
{
	/* LOE g: Load external word g */
	register ptr p = i2p(arg);

	LOG(("@L6 DoLOE(%lu)", p));
	spoilFRA();
	pushw_m(arg_g(p));
}

DoLIL(l)
	register long l;
{
	/* LIL l: Load word pointed to by l-th local or parameter */

	LOG(("@L6 DoLIL(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	pushw_m(st_lddp(loc_addr(l)));
}

DoLOF(l)
	register long l;
{
	/* LOF f: Load offsetted (top of stack + f yield address) */
	register ptr p = dppop();

	LOG(("@L6 DoLOF(%ld)", l));
	spoilFRA();
	pushw_m(p + arg_f(l));
}

DoLAL(l)
	register long l;
{
	/* LAL l: Load address of local or parameter */

	LOG(("@L6 DoLAL(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	dppush(loc_addr(l));
}

DoLAE(arg)
	register unsigned long arg;
{
	/* LAE g: Load address of external */
	register ptr p = i2p(arg);

	LOG(("@L6 DoLAE(%lu)", p));
	spoilFRA();
	dppush(arg_lae(p));
}

DoLXL(l)
	register unsigned long l;
{
	/* LXL n: Load lexical (address of LB n static levels back) */
	register ptr p;

	LOG(("@L6 DoLXL(%lu)", l));
	spoilFRA();
	l = arg_n(l);
	p = lexback_LB(l);
	dppush(p);
}

DoLXA(l)
	register unsigned long l;
{
	/* LXA n: Load lexical (address of AB n static levels back) */
	register ptr p;

	LOG(("@L6 DoLXA(%lu)", l));
	spoilFRA();
	l = arg_n(l);
	p = lexback_LB(l);
	dppush(p + rsbsize);
}

DoLOI(l)
	register size l;
{
	/* LOI o: Load indirect o bytes (address is popped from the stack) */
	register ptr p = dppop();

	LOG(("@L6 DoLOI(%ld)", l));
	spoilFRA();
	l = arg_o(l);
	push_m(p, l);
}

DoLOS(l)
	register size l;
{
	/* LOS w: Load indirect, w-byte integer on top of stack gives object size */
	register ptr p;

	LOG(("@L6 DoLOS(%ld)", l));
	spoilFRA();
	l = arg_wi(l);
	l = upop(l);
	p = dppop();
	push_m(p, arg_o(l));
}

DoLDL(l)
	register long l;
{
	/* LDL l: Load double local or parameter (two consecutive words are stacked) */

	LOG(("@L6 DoLDL(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	push_st(loc_addr(l), dwsize);
}

DoLDE(arg)
	register long arg;
{
	/* LDE g: Load double external (two consecutive externals are stacked) */
	register ptr p = i2p(arg);

	LOG(("@L6 DoLDE(%lu)", p));
	spoilFRA();
	push_m(arg_g(p), dwsize);
}

DoLDF(l)
	register long l;
{
	/* LDF f: Load double offsetted (top of stack + f yield address) */
	register ptr p = dppop();

	LOG(("@L6 DoLDF(%ld)", l));
	spoilFRA();
	push_m(p + arg_f(l), dwsize);
}

DoLPI(pi)
	register long pi;
{
	/* LPI p: Load procedure identifier */

	LOG(("@L6 DoLPI(%ld)", pi));
	spoilFRA();
	npush(arg_p(pi), psize);
}

PRIVATE ptr lexback_LB(n)
	unsigned long n;
{
	/* LB n static levels back */
	register ptr lb = LB;
	
	while (n != 0) {
		lb = st_lddp(lb + rsbsize);
		n--;
	}
	return lb;
}

