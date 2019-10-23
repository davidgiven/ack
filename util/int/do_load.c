/** @file
 *  Sources of the "LOAD" group instructions
 */

/* $Id$ */

#include	"em_abs.h"
#include	"global.h"
#include	"log.h"
#include	"mem.h"
#include	"trap.h"
#include	"text.h"
#include	"fra.h"
#include	"rsb.h"
#include	"warn.h"
#include	"switch.h"

PRIVATE ptr lexback_LB(unsigned long);

/** LOC c: Load constant (i.e. push one word onto the stack) */
void DoLOC(register long l)
{
	LOG(("@L6 DoLOC(%ld)", l));
	spoilFRA();
	wpush(arg_c(l));
}

/** LDC d: Load double constant ( push two words ) */
void DoLDC(register long l)
{
	LOG(("@L6 DoLDC(%ld)", l));
	spoilFRA();
	l = arg_d(l);
	npush(l, dwsize);
}

/** LOL l: Load word at l-th local (l<0) or parameter (l>=0) */
void DoLOL(register long l)
{
	LOG(("@L6 DoLOL(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	pushw_st(loc_addr(l));
}

/** LOE g: Load external word g */
void DoLOE(register long arg)
{
	register ptr p = i2p(arg);

	LOG(("@L6 DoLOE(%lu)", p));
	spoilFRA();
	pushw_m(arg_g(p));
}

/** LIL l: Load word pointed to by l-th local or parameter */
void DoLIL(register long l)
{
	LOG(("@L6 DoLIL(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	pushw_m(st_lddp(loc_addr(l)));
}

/** LOF f: Load offsetted (top of stack + f yield address) */
void DoLOF(register long l)
{
	register ptr p = dppop();

	LOG(("@L6 DoLOF(%ld)", l));
	spoilFRA();
	pushw_m(p + arg_f(l));
}

/** LAL l: Load address of local or parameter */
void DoLAL(register long l)
{


	LOG(("@L6 DoLAL(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	dppush(loc_addr(l));
}

/** LAE g: Load address of external */
void DoLAE(register unsigned long arg)
{

	register ptr p = i2p(arg);

	LOG(("@L6 DoLAE(%lu)", p));
	spoilFRA();
	dppush(arg_lae(p));
}

/** LXL n: Load lexical (address of LB n static levels back) */
void DoLXL(register unsigned long l)
{
	register ptr p;

	LOG(("@L6 DoLXL(%lu)", l));
	spoilFRA();
	l = arg_n(l);
	p = lexback_LB(l);
	dppush(p);
}

/** LXA n: Load lexical (address of AB n static levels back) */
void DoLXA(register unsigned long l)
{
	register ptr p;

	LOG(("@L6 DoLXA(%lu)", l));
	spoilFRA();
	l = arg_n(l);
	p = lexback_LB(l);
	dppush(p + rsbsize);
}

/** LOI o: Load indirect o bytes (address is popped from the stack) */
void DoLOI(register size l)
{
	register ptr p = dppop();

	LOG(("@L6 DoLOI(%ld)", l));
	spoilFRA();
	l = arg_o(l);
	push_m(p, l);
}

/** LOS w: Load indirect, w-byte integer on top of stack gives
 *  object size */
void DoLOS(register size l)
{
	register ptr p;

	LOG(("@L6 DoLOS(%ld)", l));
	spoilFRA();
	l = arg_wi(l);
	l = upop(l);
	p = dppop();
	push_m(p, arg_o(l));
}

/** LDL l: Load double local or parameter (two consecutive words are stacked) */
void DoLDL(register long l)
{
	LOG(("@L6 DoLDL(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	push_st(loc_addr(l), dwsize);
}

/** LDE g: Load double external (two consecutive externals are stacked) */
void DoLDE(register long arg)
{
	register ptr p = i2p(arg);

	LOG(("@L6 DoLDE(%lu)", p));
	spoilFRA();
	push_m(arg_g(p), dwsize);
}

/** LDF f: Load double offsetted (top of stack + f yield address) */
void DoLDF(register long l)
{
	register ptr p = dppop();

	LOG(("@L6 DoLDF(%ld)", l));
	spoilFRA();
	push_m(p + arg_f(l), dwsize);
}

/** LPI p: Load procedure identifier */
void DoLPI(register long pi)
{
	LOG(("@L6 DoLPI(%ld)", pi));
	spoilFRA();
	npush(arg_p(pi), psize);
}

PRIVATE ptr lexback_LB(unsigned long n)
{
	/* LB n static levels back */
	register ptr lb = LB;
	
	while (n != 0) {
		lb = st_lddp(lb + rsbsize);
		n--;
	}
	return lb;
}

