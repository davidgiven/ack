/*
 * Sources of the "POINTER ARITHMETIC" group instructions
 */

/* $Header$ */

#include	<em_abs.h>
#include	"segcheck.h"
#include	"global.h"
#include	"log.h"
#include	"mem.h"
#include	"trap.h"
#include	"warn.h"
#include	"text.h"
#include	"fra.h"

#define	adp(p,w)	((p) + (w))
#define	sbs(t,s)	((s) - (t))

#ifdef	SEGCHECK

#define	check_seg(s1,s2,w)	if (s1 != s2) { warning(w); }

#else

#define	check_seg(s1,s2,w)

#endif	SEGCHECK

DoADPl2(arg)
	long arg;
{
	/* ADP f: Add f to pointer on top of stack */
	register long l = (L_arg_2() * arg);
	register ptr p, t = st_lddp(SP);

	LOG(("@R6 DoADPl2(%ld)", l));
	spoilFRA();
	if (t == 0) {
		warning(WNULLPA);
	}
	l = arg_f(l);
	p = adp(t, l);
	check_seg(ptr2seg(t), ptr2seg(p), WSEGADP);
	st_stdp(SP, p);
}

DoADPl4(arg)
	long arg;
{
	/* ADP f: Add f to pointer on top of stack */
	register long l = (L_arg_4() * arg);
	register ptr p, t = st_lddp(SP);

	LOG(("@R6 DoADPl4(%ld)", l));
	spoilFRA();
	if (t == 0) {
		warning(WNULLPA);
	}
	l = arg_f(l);
	p = adp(t, l);
	check_seg(ptr2seg(t), ptr2seg(p), WSEGADP);
	st_stdp(SP, p);
}

DoADPm(arg)
	long arg;
{
	/* ADP f: Add f to pointer on top of stack */
	register long l = arg_f(arg);
	register ptr p, t = st_lddp(SP);

	LOG(("@R6 DoADPm(%ld)", l));
	spoilFRA();
	if (t == 0) {
		warning(WNULLPA);
	}
	l = arg_f(l);
	p = adp(t, l);
	check_seg(ptr2seg(t), ptr2seg(p), WSEGADP);
	st_stdp(SP, p);
}

DoADPs(hob, wfac)
	long hob;
	size wfac;
{
	/* ADP f: Add f to pointer on top of stack */
	register long l = (S_arg(hob) * wfac);
	register ptr p, t = st_lddp(SP);

	LOG(("@R6 DoADPs(%ld)", l));
	spoilFRA();
	if (t == 0) {
		warning(WNULLPA);
	}
	l = arg_f(l);
	p = adp(t, l);
	check_seg(ptr2seg(t), ptr2seg(p), WSEGADP);
	st_stdp(SP, p);
}

DoADSl2(arg)
	size arg;
{
	/* ADS w: Add w-byte value and pointer */
	register size l = (L_arg_2() * arg);
	register long t = spop(arg_wi(l));
	register ptr p, s = st_lddp(SP);

	LOG(("@R6 DoADSl2(%ld)", l));
	spoilFRA();
	t = arg_f(t);
	if (s == 0) {
		warning(WNULLPA);
	}
	p = adp(s, t);
	check_seg(ptr2seg(s), ptr2seg(p), WSEGADP);
	st_stdp(SP, p);
}

DoADSm(arg)
	size arg;
{
	/* ADS w: Add w-byte value and pointer */
	register long t = spop(arg_wi(arg));
	register ptr p, s = st_lddp(SP);

	LOG(("@R6 DoADSm(%ld)", arg));
	spoilFRA();
	t = arg_f(t);
	if (s == 0) {
		warning(WNULLPA);
	}
	p = adp(s, t);
	check_seg(ptr2seg(s), ptr2seg(p), WSEGADP);
	st_stdp(SP, p);
}


DoADSz()
{
	/* ADS w: Add w-byte value and pointer */
	register size l = uwpop();
	register long t = spop(arg_wi(l));
	register ptr p, s = st_lddp(SP);

	LOG(("@R6 DoADSz(%ld)", l));
	spoilFRA();
	t = arg_f(t);
	if (s == 0) {
		warning(WNULLPA);
	}
	p = adp(s, t);
	check_seg(ptr2seg(s), ptr2seg(p), WSEGADP);
	st_stdp(SP, p);
}

DoSBSl2(arg)
	size arg;
{
	/* SBS w: Subtract pointers in same fragment and push diff as size w integer */
	register size l = (L_arg_2() * arg);
	register ptr t = st_lddp(SP);
	register ptr s = st_lddp(SP + psize);
	register long w;

	LOG(("@R6 DoSBSl2(%ld)", l));
	spoilFRA();
	l = arg_wi(l);
	check_seg(ptr2seg(t), ptr2seg(s), WSEGSBS);
	w = sbs(t, s);
	if (must_test && !(IgnMask&BIT(EIOVFL))) {
		if (l == 2 && (w < I_MINS2 || w > I_MAXS2))
			trap(EIOVFL);
	}
	dppop();
	dppop();
	npush(w, l);
}

DoSBSz()
{
	/* SBS w: Subtract pointers in same fragment and push diff as size w integer */
	register size l = uwpop();
	register ptr t = st_lddp(SP);
	register ptr s = st_lddp(SP + psize);
	register long w;

	LOG(("@R6 DoSBSz(%ld)", l));
	spoilFRA();
	l = arg_wi(l);
	check_seg(ptr2seg(t), ptr2seg(s), WSEGSBS);
	w = sbs(t, s);
	if (must_test && !(IgnMask&BIT(EIOVFL))) {
		if (l == 2 && (w < I_MINS2 || w > I_MAXS2))
			trap(EIOVFL);
	}
	dppop();
	dppop();
	npush(w, l);
}
