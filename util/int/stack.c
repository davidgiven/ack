/** \file
    Stack manipulation routines.
*/

/* $Id$ */

#include	<stdio.h>

#include	"em_abs.h"
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
#include	"stack.h"
#include	"data.h"
#include	"rsb.h"

/** initial stack size in bytes */
#define	STACKSIZE	1000L

extern size maxstack; /* from main.c */

#ifdef	LOGGING
char *stack_sh; 	/* stadowbytes */
char *stackML_sh; 	/* speed up access of stadowbytes */
PRIVATE void st_clear_area(ptr, ptr);
#endif	/* LOGGING */

PRIVATE void warn_stbits(ptr, size);

/** Initialize and allocate the operand stack space "stack". */
void init_stack(void)
{
	ML = max_addr; /* set Memory Limit */
	SP = ML + 1; /* initialize Stack Pointer */
	SL = ML + 1; /* initialize Stack Limit */
	LB = ML + 1; /* initialize Local Base */
	AB = ML + 1; /* initialize Actual Base */

	SL = ML + 1 - STACKSIZE; /* initialize Stack Limit */
	stack = Malloc(STACKSIZE, "stack space");
	stackML = stack + ML;
#ifdef	LOGGING
	stack_sh = Malloc(STACKSIZE, "shadowspace for stack");
	stackML_sh = stack_sh + ML;
	st_clear_area(ML, SL);
#endif	/* LOGGING */
}

/************************************************************************
 *	EM-register division.						*
 ************************************************************************
 *									*
 *	newSP(p)	- check and adjust StackPointer.		*
 *	incSP(n)	- increment stack pointer. n already checked	*
 *	decSP(n)	- decrement stack pointer. n already checked	*
 *	newLB(p)	- check and adjust Local Base and Actual Base	*
 *									*
 ************************************************************************/

/** Set the value of the stack pointer "SP" to the value "ap".
 *  Full validation of the new value is done beforehand.
 */
void newSP(ptr ap)
{
	register ptr p = ap;

	LOG(("@s6 newSP(%lu), ML = %lu, SP = %lu", p, ML, SP));
	if (LB < p)
	{
		wtrap(WSPGTLB, ESTACK);
	}
	if (!is_wordaligned(p))
	{
		wtrap(WSPODD, ESTACK);
	}
	if (p < SP)
	{
		if (p < HP)
		{
			wtrap(WSPINHEAP, ESTACK);
		}
		if (maxstack)
		{
			/* more than allowed on command line */
			if (ML - p > maxstack)
			{
				warning(WESTACK);
				trap(ESTACK);
			}
		}
		if (p < SL)
		{
			/* extend stack space */
			register size stacksize = ML + 1 - p;

			stacksize = allocfrac(stacksize);
			SL = ML + 1 - stacksize;
			stack = Realloc(stack, (size) (stacksize), "stack space");
			stackML = stack + ML;
#ifdef	LOGGING
			stack_sh = Realloc(stack_sh, (size) (stacksize),
					"shadowspace for stack");
			stackML_sh = stack_sh + ML;
#endif	/* LOGGING */
		}

#ifdef	LOGGING
		st_clear_area(SP - 1, p);
#endif	/* LOGGING */
	}
	SP = p;
}

/** Increment stack pointer "SP" by "n" bytes.
 * Full validation on stack alignment and address is done.
 */
void incSP(size n)
{
	register ptr p = SP - n;

	if (p < HP || maxstack || p < SL)
		newSP(p);
	else
	{
		LOG(("@s6 newSP(%lu), ML = %lu, SP = %lu", p, ML, SP));
#ifdef LOGGING
		/* inline version of st_clear_area.
		 */
		SP = p;
		{
			while (n--)
			{
				st_undef(p);
				p++;
			}
		}
#endif
	}
}

/** Decrement stack pointer "SP" by "n" bytes.
 *  Full validation on stack alignment and address is done.
 */
void decSP(size n)
{
	register ptr p = SP + n;

	if (LB < p)
		newSP(p);
	else
	{
		LOG(("@s6 newSP(%lu), ML = %lu, SP = %lu", p, ML, SP));
		SP = p;
	}
}

void newLB(ptr p)
{
	if (!in_stack(p))
	{
		wtrap(WLBOUT, ESTACK);
	}
	if (!is_wordaligned(p))
	{
		wtrap(WLBODD, ESTACK);
	}
	if (!is_LB(p))
	{
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
 *	st_stw(addr, l)		- STore wordsize integer.		*
 *	st_stf(addr, f, n)	- STore Floating point number.		*
 *									*
 ************************************************************************/

/** Store data pointer "ap" in stack at address "addr".
 * Full validation is done on "addr" before storing into it.
 */
void st_stdp(register ptr addr, ptr ap)
{
	register int i;
	register long p = (long) ap;

	LOG(("@s6 st_stdp(%lu, %lu)", addr, p));
	ch_in_stack(addr, psize);
	ch_wordaligned(addr);
	for (i = (int) psize; i > 0; i--, addr++)
	{
		ch_st_prot(addr);
		stack_loc(addr) = (char) (p);
		st_dp(addr);
		p = p >> 8;
	}

}

/** Store code pointer "ap" in stack address "addr".
 *  Full validation is done on "addr" before storing into it.
 */
void st_stip(register ptr addr, ptr ap)
{
	register int i;
	register long p = (long) ap;

	LOG(("@s6 st_stip(%lu, %lu)", addr, p));
	ch_in_stack(addr, psize);
	ch_wordaligned(addr);
	for (i = (int) psize; i > 0; i--, addr++)
	{
		ch_st_prot(addr);
		stack_loc(addr) = (char) (p);
		st_ip(addr);
		p = p >> 8;
	}
}

/** Store an integer value "al" of "n" bytes in size in stack at address "addr".
 *  Full validation is done on "addr" before storing into it.
 */
void st_stn(register ptr addr, long al, size n)
{
	register int i;
	register long l = al;
#ifdef LOGGING
	/* a psize zero is ambiguous */
	int sh_flags = (l == 0 && n == psize) ? (SH_INT | SH_DATAP) : SH_INT;
#endif

	LOG(("@s6 st_stn(%lu, %ld, %lu)", addr, l, n));
	ch_in_stack(addr, n);
	ch_aligned(addr, n);

	/* store the bytes */
	for (i = (int) n; i > 0; i--, addr++)
	{
		ch_st_prot(addr);
		stack_loc(addr) = (char) l;
#ifdef	LOGGING
		st_sh(addr) = sh_flags;
#endif	/* LOGGING */
		l = l >> 8;
	}
}

/** Store an integer value "al" of word size bytes in stack at address "addr".
 *  Full validation is done on "addr" before storing into it.
 */
void st_stw(register ptr addr, long al)
{
	register int i;
	register long l = al;
#ifdef LOGGING
	/* a psize zero is ambiguous */
	int sh_flags = (l == 0 && wsize == psize) ? (SH_INT | SH_DATAP) : SH_INT;
#endif

	LOG(("@s6 st_stw(%lu, %ld)", addr, l));
	ch_w_in_stack(addr);
	ch_wordaligned(addr);

	/* store the bytes */
	for (i = (int) wsize; i > 0; i--, addr++)
	{
		ch_st_prot(addr);
		stack_loc(addr) = (char) l;
#ifdef	LOGGING
		st_sh(addr) = sh_flags;
#endif	/* LOGGING */
		l = l >> 8;
	}
}

#ifndef	NOFLOAT
/** Store a real value "f" of "n" bytes in size in stack at address "addr".
 *  Full validation is done on "addr" before storing into it.
 */
void st_stf(register ptr addr, double f, size n)
{
	register char *cp = (char *) &f;
	float fl;
	register int i;

	LOG(("@s6 st_stf(%lu, %g, %lu)", addr, f, n));
	ch_in_stack(addr, n);
	ch_wordaligned(addr);
	if ((int) n == 4)
	{
		fl = f;
		cp = (char *) &fl;
	}
	for (i = (int) n; i > 0; i--, addr++)
	{
		ch_st_prot(addr);
		stack_loc(addr) = *(cp++);
		st_fl(addr);
	}
}
#endif	/* NOFLOAT */

/************************************************************************
 *	Stack load division.						*
 ************************************************************************
 *									*
 *	st_lddp(addr)	- LoaD Data Pointer from stack.			*
 *	st_ldip(addr)	- LoaD Instruction Pointer from stack.		*
 *	st_ldu(addr, n)	- LoaD n Unsigned bytes from stack.		*
 *	st_lduw(addr)	- LoaD wsize Unsigned bytes from stack.		*
 *	st_lds(addr, n)	- LoaD n Signed bytes from stack.		*
 *	st_ldsw(addr)	- LoaD wsize Signed bytes from stack.		*
 *	st_ldf(addr, n)	- LoaD Floating point number from stack.	*
 *									*
 ************************************************************************/

/** Loads and returns a data pointer stored on the stack
 * at address "addr".
 */
ptr st_lddp(register ptr addr)
{
	register ptr p;

	LOG(("@s6 st_lddp(%lu)", addr));

	ch_in_stack(addr, psize);
	ch_wordaligned(addr);
#ifdef	LOGGING
	if (!is_st_set(addr, psize, SH_DATAP))
	{
		warning(WLDPEXP);
		warn_stbits(addr, psize);
	}
#endif	/* LOGGING */

	p = p_in_stack(addr);
	LOG(("@s6 st_lddp() returns %lu", p));
	return (p);
}

/** Loads and returns a core pointer stored on the stack
 * at address "addr".
 */
ptr st_ldip(register ptr addr)
{
	register ptr p;

	LOG(("@s6 st_ldip(%lu)", addr));

	ch_in_stack(addr, psize);
	ch_wordaligned(addr);
#ifdef	LOGGING
	if (!is_st_set(addr, psize, SH_INSP))
	{
		warning(WLIPEXP);
		warn_stbits(addr, psize);
	}
#endif	/* LOGGING */

	p = p_in_stack(addr);
	LOG(("@s6 st_ldip() returns %lu", p));
	return (p);
}

/** Loads and returns an unsigned integer value of
 * "n" bytes in size stored in the stack at address
 * "addr".
 */
unsigned long st_ldu(register ptr addr, size n)
{
	register int i;
	register unsigned long u = 0;

	LOG(("@s6 st_ldu(%lu, %lu)", addr, n));

	ch_in_stack(addr, n);
	ch_aligned(addr, n);
#ifdef	LOGGING
	if (!is_st_set(addr, n, SH_INT))
	{
		warning(n == 1 ? WLCEXP : WLIEXP);
		warn_stbits(addr, n);
	}
#endif	/* LOGGING */

	addr += n - 1;
	for (i = (int) n - 1; i >= 0; i--, addr--)
	{
		u = (u << 8) | (btou(stack_loc(addr)));
	}
	LOG(("@s6 st_ldu() returns %ld", u));
	return (u);
}

/** Loads and returns an unsigned integer value of
 * word size bytes stored in the stack at address
 * "addr".
 */
unsigned long st_lduw(register ptr addr)
{
	register int i;
	register unsigned long u = 0;

	LOG(("@s6 st_lduw(%lu)", addr));

	ch_w_in_stack(addr);
	ch_wordaligned(addr);
#ifdef	LOGGING
	if (!is_st_set(addr, wsize, SH_INT))
	{
		warning(WLIEXP);
		warn_stbits(addr, wsize);
	}
#endif	/* LOGGING */

	addr += wsize - 1;
	for (i = (int) wsize - 1; i >= 0; i--, addr--)
	{
		u = (u << 8) | (btou(stack_loc(addr)));
	}
	LOG(("@s6 st_lduw() returns %ld", u));
	return (u);
}

/** Loads and returns a signed integer value of
 * "n" bytes in size stored in the stack at address
 * "addr".
 */
long st_lds(register ptr addr, size n)
{
	register int i;
	register long l;

	LOG(("@s6 st_lds(%lu, %lu)", addr, n));

	ch_in_stack(addr, n);
	ch_aligned(addr, n);
#ifdef	LOGGING
	if (!is_st_set(addr, n, SH_INT))
	{
		warning(n == 1 ? WLCEXP : WLIEXP);
		warn_stbits(addr, n);
	}
#endif	/* LOGGING */

	addr += n - 2;
	l = btos(stack_loc(addr + 1));
	for (i = n - 2; i >= 0; i--, addr--)
	{
		l = (l << 8) | btol(stack_loc(addr));
	}
	LOG(("@s6 st_lds() returns %ld", l));
	return (l);
}

/** Loads and returns a signed integer value of
 * word size bytes stored in the stack at address
 * "addr".
 */
long st_ldsw(register ptr addr)
{
	register int i;
	register long l;

	LOG(("@s6 st_ldsw(%lu)", addr));

	ch_w_in_stack(addr);
	ch_wordaligned(addr);
#ifdef	LOGGING
	if (!is_st_set(addr, wsize, SH_INT))
	{
		warning(WLIEXP);
		warn_stbits(addr, wsize);
	}
#endif	/* LOGGING */

	addr += wsize - 2;
	l = btos(stack_loc(addr+1));
	for (i = wsize - 2; i >= 0; i--, addr--)
	{
		l = (l << 8) | btol(stack_loc(addr));
	}
	LOG(("@s6 st_ldsw() returns %ld", l));
	return (l);
}

#ifndef	NOFLOAT
/** Loads and returns a real value of "n" bytes
 * stored in the stack at address "addr".
 */
double st_ldf(register ptr addr, size n)
{
	double f;
	float fl;
	register char *cp;
	register int i;

	LOG(("@s6 st_ldf(%lu, %lu)", addr, n));

	if ((int) n == 4)
	{
		cp = (char *) &fl;
	}
	else
	{
		cp = (char *) &f;
	}
	ch_in_stack(addr, n);
	ch_wordaligned(addr);
#ifdef	LOGGING
	if (!is_st_set(addr, n, SH_FLOAT))
	{
		warning(WLFEXP);
		warn_stbits(addr, n);
	}
#endif	/* LOGGING */

	for (i = (int) n; i > 0; i--, addr++)
	{
		*(cp++) = stack_loc(addr);
	}
	if ((int) n == 4)
	{
		f = fl;
	}
	return (f);
}
#endif	/* NOFLOAT */

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

/** Moves "n" bytes from stack address "s1" to
 * stack address "s2".
 */
void st_mvs(register ptr s2, register ptr s1, size n)
	/* s1 -> s2 */
{
	register int i;

	ch_in_stack(s1, n);
	ch_wordaligned(s1);
	ch_in_stack(s2, n);
	ch_wordaligned(s2);

	for (i = (int) n; i > 0; i--, s1++, s2++)
	{
		ch_st_prot(s2);
		ch_st_prot(s1);
		stack_loc(s2) = stack_loc(s1);
#ifdef	LOGGING
		st_sh(s2) = st_sh(s1) & ~SH_PROT;
#endif	/* LOGGING */
	}
}

/** Move "n" bytes from data pointer "d" to
 * stack address "s".
 */
void st_mvd(ptr s, ptr d, size n)
	/* d -> s */
{
	register int i;

	ch_in_data(d, n);
	ch_wordaligned(d);
	ch_in_stack(s, n);
	ch_wordaligned(s);

	for (i = (int) n; i > 0; i--, s++, d++)
	{
		ch_st_prot(s);
		stack_loc(s) = data_loc(d);
#ifdef	LOGGING
		st_sh(s) = dt_sh(d) & ~SH_PROT;
#endif	/* LOGGING */
	}
}

/************************************************************************
 *	Stack pop division.						*
 ************************************************************************
 *									*
 *	dppop()		- pop a data ptr, return a ptr.			*
 *	upop(n)		- pop n unsigned bytes, return a long.		*
 *	uwpop()		- pop wsize unsigned bytes, return a long.	*
 *	spop(n)		- pop n signed bytes, return a long.		*
 *	swpop()		- pop wsize signed bytes, return a long.	*
 *	pop_dt(d, n)	- pop n bytes, store at address d in data.	*
 *	popw_dt(d)	- pop wsize bytes, store at address d in data.	*
 *	pop_st(s, n)	- pop n bytes, store at address s in stack.	*
 *	popw_st(s)	- pop wsize bytes, store at address s in stack.	*
 *	fpop()		- pop a floating point number.			*
 *	wpop()		- pop a signed word, don't care about any type.	*
 *									*
 ************************************************************************/

/** Pop and return as a data pointer from the stack. */
ptr dppop(void)
{
	register ptr p;

	p = st_lddp(SP);
	decSP(psize);
	LOG(("@s7 dppop(), return: %lu", p));
	return (p);
}

/** Pop and return as an unsigned integer "n" bytes from the stack. */
unsigned long upop(size n)
{
	register unsigned long l;

	l = st_ldu(SP, n);
	decSP(max(n, wsize));
	LOG(("@s7 upop(), return: %lu", l));
	return (l);
}

/** Pop and return a word size unsigned integer from the stack. */
unsigned long uwpop(void)
{
	register unsigned long l;

	l = st_lduw(SP);
	decSP(wsize);
	LOG(("@s7 uwpop(), return: %lu", l));
	return (l);
}

/** Pop and return as an integer "n" bytes from the stack. */
long spop(size n)
{
	register long l;

	l = st_lds(SP, n);
	decSP(max(n, wsize));
	LOG(("@s7 spop(), return: %ld", l));
	return (l);
}

/** Pop and return a word size signed integer from the stack. */
long swpop(void)
{
	register long l;

	l = st_ldsw(SP);
	decSP(wsize);
	LOG(("@s7 swpop(), return: %ld", l));
	return (l);
}

/** Pop "n" bytes from the stack and store them at data pointer
 *  address "d".
 */
void pop_dt(ptr d, size n)
{
	if (n < wsize)
		dt_stn(d, (long) upop(n), n);
	else
	{
		dt_mvs(d, SP, n);
		decSP(n);
	}
}

/** Pop word size bytes from the stack and store them at data pointer
 *  address "d".
 */
void popw_dt(ptr d)
{
	dt_mvs(d, SP, wsize);
	decSP(wsize);
}

/** Pop "n" bytes from the stack and store them at stack address "s". */
void pop_st(ptr s, size n)
{
	if (n < wsize)
		st_stn(s, (long) upop(n), n);
	else
	{
		st_mvs(s, SP, n);
		decSP(n);
	}
}

/** Pop word size bytes from the stack and store them at stack
 *  address "s".
 */
void popw_st(ptr s)
{
	st_mvs(s, SP, wsize);
	decSP(wsize);
}

#ifndef	NOFLOAT
/** Pop a real value of "n" bytes from the stack. */
double fpop(size n)
{
	double d;

	d = st_ldf(SP, n);
	decSP(n);
	return (d);
}
#endif	/* NOFLOAT */

/** Pop a word size value, independently of its type. */
long wpop(void)
{
	register long l;

	l = w_in_stack(SP);
	decSP(wsize);
	return (l);
}

/************************************************************************
 *	Stack push division.						*
 ************************************************************************
 *									*
 *	dppush(p)	- push a data ptr, load from p.			*
 *	wpush(l)	- push a word, load from l.			*
 *	npush(l, n)	- push n bytes, load from l.			*
 *	push_dt(d, n)	- push n bytes, load from address d in data.	*
 *	pushw_dt(d)	- push wsize bytes, load from address d in data.*
 *	push_st(s, n)	- push n bytes, load from address s in stack.	*
 *	pushw_st(s)	- push wsize bytes, load from address s in stack.*
 *	fpush(f, n)	- push a floating point number, of size n.	*
 *									*
 ************************************************************************/

/** Push a data pointer "p" unto the stack. */
void dppush(ptr p)
{
	incSP(psize);
	st_stdp(SP, p);
}

/** Push a word size integer "l" unto the stack. */
void wpush(long l)
{
	incSP(wsize);
	st_stw(SP, l);
}

/** Push "n" bytes from value "l" unto the stack. */
void npush(register long l, register size n)
{
	if (n <= wsize)
	{
		incSP(wsize);
		if (n == 1)
			l &= MASK1;
		else if (n == 2)
			l &= MASK2;
		st_stw(SP, l);
	}
	else
	{
		incSP(n);
		st_stn(SP, l, n);
	}
}

/** Push "n" bytes of data pointed to by the
 *  data pointer "d" unto the stack.
 */
void push_dt(ptr d, size n)
{
	if (n < wsize)
	{
		npush((long) dt_ldu(d, n), n);
	}
	else
	{
		incSP(n);
		st_mvd(SP, d, n);
	}
}

/** Push word size bytes of data pointed to by
 * the data pointer "d" unto the stack.
 */
void pushw_dt(ptr d)
{
	incSP(wsize);
	st_mvd(SP, d, wsize);
}

/** Push "n" bytes of data pointed to by the
 * stack pointer "s" unto the stack.
 */
void push_st(ptr s, size n)
{
	if (n < wsize)
	{
		npush((long) st_ldu(s, n), n);
	}
	else
	{
		incSP(n);
		st_mvs(SP, s, n);
	}
}

/** Push word size bytes of data pointed to by
 * the stack pointer "s" unto the stack.
 */
void pushw_st(ptr s)
{
	incSP(wsize);
	st_mvs(SP, s, wsize);
}

#ifndef	NOFLOAT
/** Push a real value of "n" bytes unto the stack. */
void fpush(double f, size n)
{
	incSP(n);
	st_stf(SP, f, n);
}
#endif	/* NOFLOAT */

#ifdef	LOGGING

PRIVATE void warn_stbits(ptr addr, size n)
{
	register int or_bits = 0;
	register int and_bits = 0xff;

	while (n--)
	{
		or_bits |= st_sh(addr);
		and_bits &= st_sh(addr);
		addr++;
	}

	if (or_bits != and_bits)
	{
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

PRIVATE void st_clear_area(ptr from, ptr to)
{
	/* includes both *from and *to (since ML+1 is unexpressible) */
	register ptr a;

	for (a = from; a >= to; a--) {
		st_undef(a);
	}
}


#endif	/* LOGGING */

