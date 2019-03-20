/** @file
 *  Sources of the "LOGICAL" group instructions
 */

/* $Id$ */

#include	"em_abs.h"
#include	"logging.h"
#include	"global.h"
#include	"log.h"
#include	"warn.h"
#include	"mem.h"
#include	"shadow.h"
#include	"trap.h"
#include	"text.h"
#include	"fra.h"

#ifdef	LOGGING
extern int must_test;
#endif	/* LOGGING */

#ifdef	LOGGING
#define	check_def(p,l)	if (!st_sh(p) || !st_sh(p+l)) {warning(WUNLOG);}
#else
#define	check_def(p,l)
#endif	/* LOGGING */

/** AND w: Boolean and on two groups of w bytes. Size of objects to be compared
 *  (in bytes) on top of stack
 */
void DoAND(register size l)
{
	register ptr p;

	LOG(("@X6 DoAND(%ld)", l));
	spoilFRA();
	l = arg_w(l);
	for (p = SP; p < (SP + l); p++) {
		check_def(p, l);
		stack_loc(p + l) &= stack_loc(p);
	}
	st_dec(l);
}

/** IOR w: Boolean inclusive or on two groups of w bytes */
void DoIOR(register size l)
{
	register ptr p;

	LOG(("@X6 DoIOR(%ld)", l));
	spoilFRA();
	l = arg_w(l);
	for (p = SP; p < (SP + l); p++) {
		check_def(p, l);
		stack_loc(p + l) |= stack_loc(p);
	}
	st_dec(l);
}

/** XOR w: Boolean exclusive or on two groups of w bytes */
void DoXOR(register size l)
{
	register ptr p;

	LOG(("@X6 DoXOR(%ld)", l));
	spoilFRA();
	l = arg_w(l);
	for (p = SP; p < (SP + l); p++) {
		check_def(p, l);
		stack_loc(p + l) ^= stack_loc(p);
	}
	st_dec(l);
}

/** COM w: Complement (one's complement of top w bytes) */
void DoCOM(register size l)
{
	register ptr p;

	LOG(("@X6 DoCOM(%ld)", l));
	spoilFRA();
	l = arg_w(l);
	for (p = SP; p < (SP + l); p++) {
		check_def(p, 0);
		stack_loc(p) = ~stack_loc(p);
	}
}

/** ROL w: Rotate left a group of w bytes */
void DoROL(register size l)
{
	register long s, t = uwpop();
	register long signbit;

	LOG(("@X6 DoROL(%ld)", l));
	spoilFRA();
	signbit = (arg_wi(l) == 2) ? SIGNBIT2 : SIGNBIT4;
	s = upop(l);
	
#ifdef	LOGGING
	if (must_test) {
		/* check shift distance */
		if (t < 0) {
			warning(WSHNEG);
			t = 0;
		}
		if (t >= l*8) {
			warning(WSHLARGE);
			t = l*8 - 1;
		}
	}
#endif	/* LOGGING */
	
	/* calculate result */
	while (t--) {
		s = (s & signbit) ? ((s<<1) | BIT(0)) : (s<<1);
	}
	npush(s, l);
}

/** ROR w: Rotate right a group of w bytes */
void DoROR(register size l)
{
	register long s, t = uwpop();
	register long signbit;

	LOG(("@X6 DoROR(%ld)", l));
	spoilFRA();
	signbit = (arg_wi(l) == 2) ? SIGNBIT2 : SIGNBIT4;
	s = upop(l);
	
#ifdef	LOGGING
	if (must_test) {
		/* check shift distance */
		if (t < 0) {
			warning(WSHNEG);
			t = 0;
		}
		if (t >= l*8) {
			warning(WSHLARGE);
			t = l*8 - 1;
		}
	}
#endif	/* LOGGING */
	
	/* calculate result */
	while (t--) {
		/* the >> in C does sign extension, the ROR does not */
		if (s & BIT(0))
			s = (s >> 1) | signbit;
		else	s = (s >> 1) & ~signbit;
	}
	npush(s, l);
}
