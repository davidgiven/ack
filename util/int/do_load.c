/*
 * Sources of the "LOAD" group instructions
 */

/* $Header$ */

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

DoLOCl2(arg)
	long arg;
{
	/* LOC c: Load constant (i.e. push one word onto the stack) */
	register long l = (L_arg_2() * arg);

	LOG(("@L6 DoLOCl2(%ld)", l));
	spoilFRA();
	wpush(arg_c(l));
}

DoLOCl4(arg)
	long arg;
{
	/* LOC c: Load constant (i.e. push one word onto the stack) */
	register long l = (L_arg_4() * arg);

	LOG(("@L6 DoLOCl4(%ld)", l));
	spoilFRA();
	wpush(arg_c(l));
}

DoLOCm(arg)
	long arg;
{
	/* LOC c: Load constant (i.e. push one word onto the stack) */
	register long l = arg_c(arg);

	LOG(("@L6 DoLOCm(%ld)", l));
	spoilFRA();
	wpush(l);
}

DoLOCs(hob, wfac)
	long hob;
	size wfac;
{
	/* LOC c: Load constant (i.e. push one word onto the stack) */
	register long l = (S_arg(hob) * wfac);

	LOG(("@L6 DoLOCs(%ld)", l));
	spoilFRA();
	wpush(arg_c(l));
}

DoLDCl2(arg)
	long arg;
{
	/* LDC d: Load double constant ( push two words ) */
	register long l = (L_arg_2() * arg);

	LOG(("@L6 DoLDCl2(%ld)", l));
	spoilFRA();
	npush(arg_d(l), dwsize);
}

DoLDCl4(arg)
	long arg;
{
	/* LDC d: Load double constant ( push two words ) */
	register long l = (L_arg_4() * arg);

	LOG(("@L6 DoLDCl4(%ld)", l));
	spoilFRA();
	npush(arg_d(l), dwsize);
}

DoLDCm(arg)
	long arg;
{
	/* LDC d: Load double constant ( push two words ) */
	register long l = arg_d(arg);

	LOG(("@L6 DoLDCm(%ld)", l));
	spoilFRA();
	npush(l, dwsize);
}

DoLOLm(arg)
	long arg;
{
	/* LOL l: Load word at l-th local (l<0) or parameter (l>=0) */
	register long l = arg_l(arg);

	LOG(("@L6 DoLOLm(%ld)", l));
	spoilFRA();
	push_st(loc_addr(l), wsize);
}

DoLOLn2(arg)
	long arg;
{
	/* LOL l: Load word at l-th local (l<0) or parameter (l>=0) */
	register long l = (N_arg_2() * arg);

	LOG(("@L6 DoLOLn2(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	push_st(loc_addr(l), wsize);
}

DoLOLn4(arg)
	long arg;
{
	/* LOL l: Load word at l-th local (l<0) or parameter (l>=0) */
	register long l = (N_arg_4() * arg);

	LOG(("@L6 DoLOLn4(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	push_st(loc_addr(l), wsize);
}

DoLOLp2(arg)
	long arg;
{
	/* LOL l: Load word at l-th local (l<0) or parameter (l>=0) */
	register long l = (P_arg_2() * arg);

	LOG(("@L6 DoLOLp2(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	push_st(loc_addr(l), wsize);
}

DoLOLp4(arg)
	long arg;
{
	/* LOL l: Load word at l-th local (l<0) or parameter (l>=0) */
	register long l = (P_arg_4() * arg);

	LOG(("@L6 DoLOLp4(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	push_st(loc_addr(l), wsize);
}

DoLOLs(hob, wfac)
	long hob;
	size wfac;
{
	/* LOL l: Load word at l-th local (l<0) or parameter (l>=0) */
	register long l = (S_arg(hob) * wfac);

	LOG(("@L6 DoLOLs(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	push_st(loc_addr(l), wsize);
}

DoLOEl2(arg)
	long arg;
{
	/* LOE g: Load external word g */
	register ptr p = i2p(L_arg_2() * arg);

	LOG(("@L6 DoLOEl2(%lu)", p));
	spoilFRA();
	push_m(arg_g(p), wsize);
}

DoLOEl4(arg)
	long arg;
{
	/* LOE g: Load external word g */
	register ptr p = i2p(L_arg_4() * arg);

	LOG(("@L6 DoLOEl4(%lu)", p));
	spoilFRA();
	push_m(arg_g(p), wsize);
}

DoLOEs(hob, wfac)
	long hob;
	size wfac;
{
	/* LOE g: Load external word g */
	register ptr p = i2p(S_arg(hob) * wfac);

	LOG(("@L6 DoLOEs(%lu)", p));
	spoilFRA();
	push_m(arg_g(p), wsize);
}

DoLILm(arg)
	long arg;
{
	/* LIL l: Load word pointed to by l-th local or parameter */
	register long l = arg_l(arg);

	LOG(("@L6 DoLILm(%ld)", l));
	spoilFRA();
	push_m(st_lddp(loc_addr(l)), wsize);
}

DoLILn2(arg)
	long arg;
{
	/* LIL l: Load word pointed to by l-th local or parameter */
	register long l = (N_arg_2() * arg);

	LOG(("@L6 DoLILn2(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	push_m(st_lddp(loc_addr(l)), wsize);
}

DoLILn4(arg)
	long arg;
{
	/* LIL l: Load word pointed to by l-th local or parameter */
	register long l = (N_arg_4() * arg);

	LOG(("@L6 DoLILn4(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	push_m(st_lddp(loc_addr(l)), wsize);
}

DoLILp2(arg)
	long arg;
{
	/* LIL l: Load word pointed to by l-th local or parameter */
	register long l = (P_arg_2() * arg);

	LOG(("@L6 DoLILp2(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	push_m(st_lddp(loc_addr(l)), wsize);
}

DoLILp4(arg)
	long arg;
{
	/* LIL l: Load word pointed to by l-th local or parameter */
	register long l = (P_arg_4() * arg);

	LOG(("@L6 DoLILp4(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	push_m(st_lddp(loc_addr(l)), wsize);
}

DoLILs(hob, wfac)
	long hob;
	size wfac;
{
	/* LIL l: Load word pointed to by l-th local or parameter */
	register long l = (S_arg(hob) * wfac);

	LOG(("@L6 DoLILs(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	push_m(st_lddp(loc_addr(l)), wsize);
}

DoLOFl2(arg)
	long arg;
{
	/* LOF f: Load offsetted (top of stack + f yield address) */
	register long l = (L_arg_2() * arg);
	register ptr p = dppop();

	LOG(("@L6 DoLOFl2(%ld)", l));
	spoilFRA();
	push_m(p + arg_f(l), wsize);
}

DoLOFl4(arg)
	long arg;
{
	/* LOF f: Load offsetted (top of stack + f yield address) */
	register long l = (L_arg_4() * arg);
	register ptr p = dppop();

	LOG(("@L6 DoLOFl4(%ld)", l));
	spoilFRA();
	push_m(p + arg_f(l), wsize);
}

DoLOFm(arg)
	long arg;
{
	/* LOF f: Load offsetted (top of stack + f yield address) */
	register long l = arg;
	register ptr p = dppop();

	LOG(("@L6 DoLOFm(%ld)", l));
	spoilFRA();
	push_m(p + arg_f(l), wsize);
}

DoLOFs(hob, wfac)
	long hob;
	size wfac;
{
	/* LOF f: Load offsetted (top of stack + f yield address) */
	register long l = (S_arg(hob) * wfac);
	register ptr p = dppop();

	LOG(("@L6 DoLOFs(%ld)", l));
	spoilFRA();
	push_m(p + arg_f(l), wsize);
}

DoLALm(arg)
	long arg;
{
	/* LAL l: Load address of local or parameter */
	register long l = arg_l(arg);

	LOG(("@L6 DoLALm(%ld)", l));
	spoilFRA();
	dppush(loc_addr(l));
}

DoLALn2(arg)
	long arg;
{
	/* LAL l: Load address of local or parameter */
	register long l = (N_arg_2() * arg);

	LOG(("@L6 DoLALn2(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	dppush(loc_addr(l));
}

DoLALn4(arg)
	long arg;
{
	/* LAL l: Load address of local or parameter */
	register long l = (N_arg_4() * arg);

	LOG(("@L6 DoLALn4(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	dppush(loc_addr(l));
}

DoLALp2(arg)
	long arg;
{
	/* LAL l: Load address of local or parameter */
	register long l = (P_arg_2() * arg);

	LOG(("@L6 DoLALp2(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	dppush(loc_addr(l));
}

DoLALp4(arg)
	long arg;
{
	/* LAL l: Load address of local or parameter */
	register long l = (P_arg_4() * arg);

	LOG(("@L6 DoLALp4(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	dppush(loc_addr(l));
}

DoLALs(hob, wfac)
	long hob;
	size wfac;
{
	/* LAL l: Load address of local or parameter */
	register long l = (S_arg(hob) * wfac);

	LOG(("@L6 DoLALs(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	dppush(loc_addr(l));
}

DoLAEu(arg)
	long arg;
{
	/* LAE g: Load address of external */
	register ptr p = i2p(U_arg() * arg);

	LOG(("@L6 DoLAEu(%lu)", p));
	spoilFRA();
	dppush(arg_lae(p));
}

DoLAEl4(arg)
	long arg;
{
	/* LAE g: Load address of external */
	register ptr p = i2p(L_arg_4() * arg);

	LOG(("@L6 DoLAEl4(%lu)", p));
	spoilFRA();
	dppush(arg_lae(p));
}

DoLAEs(hob, wfac)
	long hob;
	size wfac;
{
	/* LAE g: Load address of external */
	register ptr p = i2p(S_arg(hob) * wfac);

	LOG(("@L6 DoLAEs(%lu)", p));
	spoilFRA();
	dppush(arg_lae(p));
}

DoLXLl2(arg)
	unsigned long arg;
{
	/* LXL n: Load lexical (address of LB n static levels back) */
	register unsigned long l = (L_arg_2() * arg);
	register ptr p;

	LOG(("@L6 DoLXLl2(%lu)", l));
	spoilFRA();
	l = arg_n(l);
	p = lexback_LB(l);
	dppush(p);
}

DoLXLm(arg)
	unsigned long arg;
{
	/* LXL n: Load lexical (address of LB n static levels back) */
	register unsigned long l = arg_n(arg);
	register ptr p;

	LOG(("@L6 DoLXLm(%lu)", l));
	spoilFRA();
	p = lexback_LB(l);
	dppush(p);
}

DoLXAl2(arg)
	unsigned long arg;
{
	/* LXA n: Load lexical (address of AB n static levels back) */
	register unsigned long l = (P_arg_2() * arg);
	register ptr p;

	LOG(("@L6 DoLXAl2(%lu)", l));
	spoilFRA();
	l = arg_n(l);
	p = lexback_LB(l);
	dppush(p + rsbsize);
}

DoLXAm(arg)
	unsigned long arg;
{
	/* LXA n: Load lexical (address of AB n static levels back) */
	register unsigned long l = arg_n(arg);
	register ptr p;

	LOG(("@L6 DoLXAm(%lu)", l));
	spoilFRA();
	p = lexback_LB(l);
	dppush(p + rsbsize);
}

DoLOIl2(arg)
	size arg;
{
	/* LOI o: Load indirect o bytes (address is popped from the stack) */
	register size l = (L_arg_2() * arg);
	register ptr p = dppop();

	LOG(("@L6 DoLOIl2(%ld)", l));
	spoilFRA();
	push_m(p, arg_o(l));
}

DoLOIl4(arg)
	size arg;
{
	/* LOI o: Load indirect o bytes (address is popped from the stack) */
	register size l = (L_arg_4() * arg);
	register ptr p = dppop();

	LOG(("@L6 DoLOIl4(%ld)", l));
	spoilFRA();
	push_m(p, arg_o(l));
}

DoLOIm(arg)
	size arg;
{
	/* LOI o: Load indirect o bytes (address is popped from the stack) */
	register size l = arg_o(arg);
	register ptr p = dppop();

	LOG(("@L6 DoLOIm(%ld)", l));
	spoilFRA();
	push_m(p, l);
}

DoLOIs(hob, wfac)
	long hob;
	size wfac;
{
	/* LOI o: Load indirect o bytes (address is popped from the stack) */
	register size l = (S_arg(hob) * wfac);
	register ptr p = dppop();

	LOG(("@L6 DoLOIs(%ld)", l));
	spoilFRA();
	push_m(p, arg_o(l));
}

DoLOSl2(arg)
	size arg;
{
	/* LOS w: Load indirect, w-byte integer on top of stack gives object size */
	register size l = (P_arg_2() * arg);
	register ptr p;

	LOG(("@L6 DoLOSl2(%ld)", l));
	spoilFRA();
	l = arg_wi(l);
	l = upop(l);
	p = dppop();
	push_m(p, arg_o(l));
}

DoLOSz()
{
	/* LOS w: Load indirect, w-byte integer on top of stack gives object size */
	register size l = upop(wsize);
	register ptr p;

	LOG(("@L6 DoLOSz(%ld)", l));
	spoilFRA();
	l = arg_wi(l);
	l = upop(l);
	p = dppop();
	push_m(p, arg_o(l));
}

DoLDLm(arg)
	long arg;
{
	/* LDL l: Load double local or parameter (two consecutive words are stacked) */
	register long l = arg_l(arg);

	LOG(("@L6 DoLDLm(%ld)", l));
	spoilFRA();
	push_st(loc_addr(l), dwsize);
}

DoLDLn2(arg)
	long arg;
{
	/* LDL l: Load double local or parameter (two consecutive words are stacked) */
	register long l = (N_arg_2() * arg);

	LOG(("@L6 DoLDLn2(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	push_st(loc_addr(l), dwsize);
}

DoLDLn4(arg)
	long arg;
{
	/* LDL l: Load double local or parameter (two consecutive words are stacked) */
	register long l = (N_arg_4() * arg);

	LOG(("@L6 DoLDLn4(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	push_st(loc_addr(l), dwsize);
}

DoLDLp2(arg)
	long arg;
{
	/* LDL l: Load double local or parameter (two consecutive words are stacked) */
	register long l = (P_arg_2() * arg);

	LOG(("@L6 DoLDLp2(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	push_st(loc_addr(l), dwsize);
}

DoLDLp4(arg)
	long arg;
{
	/* LDL l: Load double local or parameter (two consecutive words are stacked) */
	register long l = (P_arg_4() * arg);

	LOG(("@L6 DoLDLp4(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	push_st(loc_addr(l), dwsize);
}

DoLDLs(hob, wfac)
	long hob;
	size wfac;
{
	/* LDL l: Load double local or parameter (two consecutive words are stacked) */
	register long l = (S_arg(hob) * wfac);

	LOG(("@L6 DoLDLs(%ld)", l));
	spoilFRA();
	l = arg_l(l);
	push_st(loc_addr(l), dwsize);

}

DoLDEl2(arg)
	long arg;
{
	/* LDE g: Load double external (two consecutive externals are stacked) */
	register ptr p = i2p(L_arg_2() * arg);

	LOG(("@L6 DoLDEl2(%lu)", p));
	spoilFRA();
	push_m(arg_g(p), dwsize);
}

DoLDEl4(arg)
	long arg;
{
	/* LDE g: Load double external (two consecutive externals are stacked) */
	register ptr p = i2p(L_arg_4() * arg);

	LOG(("@L6 DoLDEl4(%lu)", p));
	spoilFRA();
	push_m(arg_g(p), dwsize);
}

DoLDEs(hob, wfac)
	long hob;
	size wfac;
{
	/* LDE g: Load double external (two consecutive externals are stacked) */
	register ptr p = i2p(S_arg(hob) * wfac);

	LOG(("@L6 DoLDEs(%lu)", p));
	spoilFRA();
	push_m(arg_g(p), dwsize);
}

DoLDFl2(arg)
	long arg;
{
	/* LDF f: Load double offsetted (top of stack + f yield address) */
	register long l = (L_arg_2() * arg);
	register ptr p = dppop();

	LOG(("@L6 DoLDFl2(%ld)", l));
	spoilFRA();
	push_m(p + arg_f(l), dwsize);
}

DoLDFl4(arg)
	long arg;
{
	/* LDF f: Load double offsetted (top of stack + f yield address) */
	register long l = (L_arg_4() * arg);
	register ptr p = dppop();

	LOG(("@L6 DoLDFl4(%ld)", l));
	spoilFRA();
	push_m(p + arg_f(l), dwsize);
}

DoLPIl2(arg)
	long arg;
{
	/* LPI p: Load procedure identifier */
	register long pi = (L_arg_2() * arg);

	LOG(("@L6 DoLPIl2(%ld)", pi));
	spoilFRA();
	npush(arg_p(pi), psize);
}

DoLPIl4(arg)
	long arg;
{
	/* LPI p: Load procedure identifier */
	register long pi = (L_arg_4() * arg);

	LOG(("@L6 DoLPIl4(%ld)", pi));
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

