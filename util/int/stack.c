/*
	Stack manipulation
*/

/* $Header$ */

#include	<stdio.h>

#include	<em_abs.h>
#include	"logging.h"
#include	"nofloat.h"
#include	"global.h"
#include	"log.h"
#include	"warn.h"
#include	"trap.h"
#include	"alloc.h"
#include	"memdirect.h"
#include	"mem.h"
#include	"shadow.h"
#include	"rsb.h"

#define	STACKSIZE	1000L		/* initial stack size */

extern size maxstack;			/* from main.c */

#ifdef	LOGGING
char *stack_sh;				/* stadowbytes */
#endif	LOGGING

PRIVATE warn_stbits();

init_stack() {
	ML = max_addr;			/* set Memory Limit */
	SP = ML + 1;			/* initialize Stack Pointer */
	SL = ML + 1;			/* initialize Stack Limit */
	LB = ML + 1;			/* initialize Local Base */
	AB = ML + 1;			/* initialize Actual Base */

	SL = ML + 1 - STACKSIZE;	/* initialize Stack Limit */
	stack = Malloc(STACKSIZE, "stack space");
#ifdef	LOGGING
	stack_sh = Malloc(STACKSIZE, "shadowspace for stack");
	st_clear_area(ML, SL);
#endif	LOGGING
}


/************************************************************************
 *	EM-register division.						*
 ************************************************************************
 *									*
 *	newSP(p)	- check and adjust StackPointer.		*
 *	newLB(p)	- check and adjust Local Base and Actual Base	*
 *									*
 ************************************************************************/

newSP(ap)
	ptr ap;
{
	register ptr p = ap;
	
	LOG(("@s6 newSP(%lu), ML = %lu, SP = %lu", p, ML, SP));
	if (LB < p) {
		wtrap(WSPGTLB, ESTACK);
	}
	if (p < HP) {
		wtrap(WSPINHEAP, ESTACK);
	}
	if (!is_aligned(p, wsize)) {
		wtrap(WSPODD, ESTACK);
	}
	if (maxstack) {
		/* more than allowed on command line */
		if (ML - p > maxstack) {
			warning(WESTACK);
			trap(ESTACK);
		}
	}
	if (p < SL) {
		/* extend stack space */
		register size stacksize = ML + 1 - p;

		stacksize = allocfrac(stacksize);
		SL = ML + 1 - stacksize;
		stack = Realloc(stack, (size)(stacksize), "stack space");
#ifdef	LOGGING
		stack_sh = Realloc(stack_sh, (size)(stacksize),
						"shadowspace for stack");
#endif	LOGGING
	}

#ifdef	LOGGING
	if (!in_stack(p)) {
		st_clear_area(SP - 1, p);
	}
#endif	LOGGING
	SP = p;
}

newLB(p)
	ptr p;
{
	if (!in_stack(p)) {
		wtrap(WLBOUT, ESTACK);
	}
	if (!is_aligned(p, wsize)) {
		wtrap(WLBODD, ESTACK);
	}
	if (!is_LB(p)) {
		wtrap(WLBRSB, ESTACK);
	}
	LB = p;
	AB = LB + rsbsize;
}


/************************************************************************
 *	Stack store division.						*
 ************************************************************************
 *									*
 *	st_stdp(addr, p)	- STore Data Pointer.			*
 *	st_stip(addr, p)	- STore Instruction Pointer.		*
 *	st_stn(addr, l, n)	- STore N byte integer.			*
 *	st_stf(addr, f, n)	- STore Floating point number.		*
 *									*
 ************************************************************************/

st_stdp(addr, ap)
	ptr addr, ap;
{
	register int i;
	register long p = (long) ap;

	LOG(("@s6 st_stdp(%lu, %lu)", addr, p));
	ch_in_stack(addr, psize);
	ch_aligned(addr, wsize);
	for (i = 0; i < (int) psize; i++) {
		ch_st_prot(addr + i);
		stack_loc(addr + i) = (char) (p);
		st_dp(addr + i);
		p = p>>8;
	}

}

st_stip(addr, ap)
	ptr addr, ap;
{
	register int i;
	register long p = (long) ap;

	LOG(("@s6 st_stip(%lu, %lu)", addr, p));
	ch_in_stack(addr, psize);
	ch_aligned(addr, wsize);
	for (i = 0; i < (int) psize; i++) {
		ch_st_prot(addr + i);
		stack_loc(addr + i) = (char) (p);
		st_ip(addr + i);
		p = p>>8;
	}
}

st_stn(addr, al, n)
	ptr addr;
	long al;
	size n;
{
	register int i;
	register long l = al;

	LOG(("@s6 st_stn(%lu, %ld, %lu)", addr, l, n));
	ch_in_stack(addr, n);
	ch_aligned(addr, n);

	/* store the bytes */
	for (i = 0; i < (int) n; i++) {
		ch_st_prot(addr + i);
		stack_loc(addr + i) = (char) l;
#ifdef	LOGGING
		if (al == 0 && n == psize) {
			/* a psize zero, ambiguous */
			st_sh(addr + i) = (SH_INT|SH_DATAP);
		}
		else {
			st_sh(addr + i) = SH_INT;
		}
#endif	LOGGING
		l = l>>8;
	}
}

#ifndef	NOFLOAT
st_stf(addr, f, n)
	ptr addr;
	double f;
	size n;
{
	register char *cp = (char *) &f;
	register int i;

	LOG(("@s6 st_stf(%lu, %g, %lu)", addr, f, n));
	ch_in_stack(addr, n);
	ch_aligned(addr, wsize);
	for (i = 0; i < (int) n; i++) {
		ch_st_prot(addr + i);
		stack_loc(addr + i) = *(cp++);
		st_fl(addr + i);
	}
}
#endif	NOFLOAT

/************************************************************************
 *	Stack load division.						*
 ************************************************************************
 *									*
 *	st_lddp(addr)	- LoaD Data Pointer from stack.			*
 *	st_ldip(addr)	- LoaD Instruction Pointer from stack.		*
 *	st_ldu(addr, n)	- LoaD n Unsigned bytes from stack.		*
 *	st_lds(addr, n)	- LoaD n Signed bytes from stack.		*
 *	st_ldf(addr, n)	- LoaD Floating point number from stack.	*
 *									*
 ************************************************************************/

ptr st_lddp(addr)
	ptr addr;
{
	register ptr p;

	LOG(("@s6 st_lddp(%lu)", addr));

	ch_in_stack(addr, psize);
	ch_aligned(addr, wsize);
#ifdef	LOGGING
	if (!is_st_set(addr, psize, SH_DATAP)) {
		warning(WLDPEXP);
		warn_stbits(addr, psize);
	}
#endif	LOGGING

	p = p_in_stack(addr);
	LOG(("@s6 st_lddp() returns %lu", p));
	return (p);
}

ptr st_ldip(addr)
	ptr addr;
{
	register ptr p;

	LOG(("@s6 st_ldip(%lu)", addr));

	ch_in_stack(addr, psize);
	ch_aligned(addr, wsize);
#ifdef	LOGGING
	if (!is_st_set(addr, psize, SH_INSP)) {
		warning(WLIPEXP);
		warn_stbits(addr, psize);
	}
#endif	LOGGING

	p = p_in_stack(addr);
	LOG(("@s6 st_ldip() returns %lu", p));
	return (p);
}

unsigned long st_ldu(addr, n)
	ptr addr;
	size n;
{
	register int i;
	register unsigned long u = 0;

	LOG(("@s6 st_ldu(%lu, %lu)", addr, n));

	ch_in_stack(addr, n);
	ch_aligned(addr, n);
#ifdef	LOGGING
	if (!is_st_set(addr, n, SH_INT)) {
		warning(n == 1 ? WLCEXP : WLIEXP);
		warn_stbits(addr, n);
	}
#endif	LOGGING

	for (i = (int) n-1; i >= 0; i--) {
		u = (u<<8) | (btou(stack_loc(addr + i)));
	}
	LOG(("@s6 st_ldu() returns %ld", u));
	return (u);
}

long st_lds(addr, n)
	ptr addr;
	size n;
{
	register int i;
	register long l;

	LOG(("@s6 st_lds(%lu, %lu)", addr, n));

	ch_in_stack(addr, n);
	ch_aligned(addr, n);
#ifdef	LOGGING
	if (!is_st_set(addr, n, SH_INT)) {
		warning(n == 1 ? WLCEXP : WLIEXP);
		warn_stbits(addr, n);
	}
#endif	LOGGING

	l = btos(stack_loc(addr + n - 1));
	for (i = n - 2; i >= 0; i--) {
		l = (l<<8) | btol(stack_loc(addr + i));
	}
	LOG(("@s6 st_lds() returns %ld", l));
	return (l);
}

#ifndef	NOFLOAT
double st_ldf(addr, n)
	ptr addr;
	size n;
{
	double f = 0.0;
	register char *cp = (char *) &f;
	register int i;

	LOG(("@s6 st_ldf(%lu, %lu)", addr, n));

	ch_in_stack(addr, n);
	ch_aligned(addr, wsize);
#ifdef	LOGGING
	if (!is_st_set(addr, n, SH_FLOAT)) {
		warning(WLFEXP);
		warn_stbits(addr, n);
	}
#endif	LOGGING

	for (i = 0; i < (int) n; i++) {
		*(cp++) = stack_loc(addr + i);
	}
	return (f);
}
#endif	NOFLOAT

/************************************************************************
 *	Stack move division						*
 ************************************************************************
 *									*
 *	st_mvs(s2, s1, n) - Move n bytes in stack from s1 to s2.	*
 *	st_mvd(s, d, n) - Move n bytes from d in data to s in stack.	*
 *									*
 *	st_mvs(): The intention is to copy the contents of addresses	*
 *	s1, s1+1....s1-(n-1) to addresses s2, s2+1....s2+(n-1).		*
 *	All addresses are expected to be in the stack. This condition	*
 *	is checked for. The shadow bytes of the bytes to be filled in,	*
 *	are marked identical to the source-shadow bytes.		*
 *									*
 *	st_mvd(), dt_mvd() and dt_mvs() act identically (see data.c).	*
 *									*
 ************************************************************************/

st_mvs(s2, s1, n)			/* s1 -> s2 */
	ptr s2, s1;
	size n;
{
	register int i;

	ch_in_stack(s1, n);
	ch_aligned(s1, wsize);
	ch_in_stack(s2, n);
	ch_aligned(s2, wsize);

	for (i = 0; i < (int) n; i++) {
		ch_st_prot(s2 + i);
		ch_st_prot(s1 + i);
		stack_loc(s2 + i) = stack_loc(s1 + i);
#ifdef	LOGGING
		st_sh(s2 + i) = st_sh(s1 + i) & ~SH_PROT;
#endif	LOGGING
	}
}

st_mvd(s, d, n)				/* d -> s */
	ptr s, d;
	size n;
{
	register int i;

	ch_in_data(d, n);
	ch_aligned(d, wsize);
	ch_in_stack(s, n);
	ch_aligned(s, wsize);

	for (i = 0; i < (int) n; i++) {
		ch_st_prot(s + i);
		stack_loc(s + i) = data_loc(d + i);
#ifdef	LOGGING
		st_sh(s + i) = dt_sh(d + i) & ~SH_PROT;
#endif	LOGGING
	}
}

/************************************************************************
 *	Stack pop division.						*
 ************************************************************************
 *									*
 *	dppop()		- pop a data ptr, return a ptr.			*
 *	upop(n)		- pop n unsigned bytes, return a long.		*
 *	spop(n)		- pop n signed bytes, return a long.		*
 *	pop_dt(d, n)	- pop n bytes, store at address d in data.	*
 *	pop_st(s, n)	- pop n bytes, store at address s in stack.	*
 *	fpop()		- pop a floating point number.			*
 *	wpop()		- pop a signed word, don't care about any type.	*
 *									*
 ************************************************************************/

ptr dppop()
{
	register ptr p;

	p = st_lddp(SP);
	st_dec(psize);
	LOG(("@s7 dppop(), return: %lu", p));
	return (p);
}

unsigned long upop(n)
	size n;
{
	register unsigned long l;

	l = st_ldu(SP, n);
	st_dec(max(n, wsize));
	LOG(("@s7 upop(), return: %lu", l));
	return (l);
}

long spop(n)
	size n;
{
	register long l;

	l = st_lds(SP, n);
	st_dec(max(n, wsize));
	LOG(("@s7 spop(), return: %ld", l));
	return (l);
}

pop_dt(d, n)
	ptr d;
	size n;
{
	if (n < wsize)
		dt_stn(d, (long) upop(n), n);
	else {
		dt_mvs(d, SP, n);
		st_dec(n);
	}
}

pop_st(s, n)
	ptr s;
	size n;
{
	if (n < wsize)
		st_stn(s, (long) upop(n), n);
	else {
		st_mvs(s, SP, n);
		st_dec(n);
	}
}

#ifndef	NOFLOAT
double fpop(n)
	size n;
{
	double d;

	d = st_ldf(SP, n);
	st_dec(n);
	return (d);
}
#endif	NOFLOAT

long wpop()
{
	register long l;
	
	l = w_in_stack(SP);
	st_dec(wsize);
	return (l);
}

/************************************************************************
 *	Stack push division.						*
 ************************************************************************
 *									*
 *	dppush(p)	- push a data ptr, load from p.			*
 *	npush(l, n)	- push n bytes, load from l.			*
 *	push_dt(d, n)	- push n bytes, load from address d in data.	*
 *	push_st(s, n)	- push n bytes, load from address s in stack.	*
 *	fpush(f, n)	- push a floating point number, of size n.	*
 *									*
 ************************************************************************/

dppush(p)
	ptr p;
{
	st_inc(psize);
	st_stdp(SP, p);
}

npush(l, n)
	long l;
	size n;
{
	st_inc(max(n, wsize));
	if (n == 1)
		l &= MASK1;
	else
	if (n == 2)
		l &= MASK2;
	st_stn(SP, l, max(n, wsize));

}

push_dt(d, n)
	ptr d;
	size n;
{
	if (n < wsize) {
		npush((long) dt_ldu(d, n), n);
	}
	else {
		st_inc(n);
		st_mvd(SP, d, n);
	}
}

push_st(s, n)
	ptr s;
	size n;
{
	if (n < wsize) {
		npush((long) st_ldu(s, n), n);
	}
	else {
		st_inc(n);
		st_mvs(SP, s, n);
	}
}

#ifndef	NOFLOAT
fpush(f, n)
	double f;
	size n;
{
	st_inc(n);
	st_stf(SP, f, n);
}
#endif	NOFLOAT

#ifdef	LOGGING

PRIVATE warn_stbits(addr, n)
	ptr addr;
	size n;
{
	register int or_bits = 0;
	register int and_bits = 0xff;

	while (n--) {
		or_bits |= st_sh(addr);
		and_bits &= st_sh(addr);
		addr++;
	}

	if (or_bits != and_bits) {
		/* no use trying to diagnose */
		warningcont(WWASMISC);
		return;
	}
	if (or_bits == 0)
		warningcont(WWASUND);
	if (or_bits & SH_INT)
		warningcont(WWASINT);
	if (or_bits & SH_FLOAT)
		warningcont(WWASFLOAT);
	if (or_bits & SH_DATAP)
		warningcont(WWASDATAP);
	if (or_bits & SH_INSP)
		warningcont(WWASINSP);
}

#endif	LOGGING

