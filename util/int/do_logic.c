/*
 * Sources of the "LOGICAL" group instructions
 */

/* $Header$ */

#include	<em_abs.h>
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
#endif	LOGGING

#ifdef	LOGGING
#define	check_def(p,l)	if (!st_sh(p) || !st_sh(p+l)) {warning(WUNLOG);}
#else
#define	check_def(p,l)
#endif	LOGGING

DoANDl2(arg)
	size arg;
{
	/* AND w: Boolean and on two groups of w bytes */
	register size l = (L_arg_2() * arg);
	register ptr p;

	LOG(("@X6 DoANDl2(%ld)", l));
	spoilFRA();
	l = arg_w(l);
	for (p = SP; p < (SP + l); p++) {
		check_def(p, l);
		stack_loc(p + l) &= stack_loc(p);
	}
	st_dec(l);
}

DoANDm(arg)
	size arg;
{
	/* AND w: Boolean and on two groups of w bytes */
	register size l = arg_w(arg);
	register ptr p;

	LOG(("@X6 DoANDm(%ld)", l));
	spoilFRA();
	for (p = SP; p < (SP + l); p ++) {
		check_def(p, l);
		stack_loc(p + l) &= stack_loc(p);
	}
	st_dec(l);
}

DoANDz()
{
	/* AND w: Boolean and on two groups of w bytes */
	/* size of objects to be compared (in bytes) on top of stack */
	register size l = uwpop();
	register ptr p;

	LOG(("@X6 DoANDz(%ld)", l));
	spoilFRA();
	l = arg_w(l);
	for (p = SP; p < (SP + l); p++) {
		check_def(p, l);
		stack_loc(p + l) &= stack_loc(p);
	}
	st_dec(l);
}

DoIORl2(arg)
	size arg;
{
	/* IOR w: Boolean inclusive or on two groups of w bytes */
	register size l = (L_arg_2() * arg);
	register ptr p;

	LOG(("@X6 DoIORl2(%ld)", l));
	spoilFRA();
	l = arg_w(l);
	for (p = SP; p < (SP + l); p++) {
		check_def(p, l);
		stack_loc(p + l) |= stack_loc(p);
	}
	st_dec(l);
}

DoIORm(arg)
	size arg;
{
	/* IOR w: Boolean inclusive or on two groups of w bytes */
	register size l = arg_w(arg);
	register ptr p;

	LOG(("@X6 DoIORm(%ld)", l));
	spoilFRA();
	for (p = SP; p < (SP + l); p++) {
		check_def(p, l);
		stack_loc(p + l) |= stack_loc(p);
	}
	st_dec(l);
}

DoIORs(hob, wfac)
	long hob;
	size wfac;
{
	/* IOR w: Boolean inclusive or on two groups of w bytes */
	register size l = (S_arg(hob) * wfac);
	register ptr p;

	LOG(("@X6 DoIORs(%ld)", l));
	spoilFRA();
	l = arg_w(l);
	for (p = SP; p < (SP + l); p++) {
		check_def(p, l);
		stack_loc(p + l) |= stack_loc(p);
	}
	st_dec(l);
}

DoIORz()
{
	/* IOR w: Boolean inclusive or on two groups of w bytes */
	register size l = uwpop();
	register ptr p;

	LOG(("@X6 DoIORz(%ld)", l));
	spoilFRA();
	l = arg_w(l);
	for (p = SP; p < (SP + l); p++) {
		check_def(p, l);
		stack_loc(p + l) |= stack_loc(p);
	}
	st_dec(l);
}

DoXORl2(arg)
	size arg;
{
	/* XOR w: Boolean exclusive or on two groups of w bytes */
	register size l = (L_arg_2() * arg);
	register ptr p;

	LOG(("@X6 DoXORl2(%ld)", l));
	spoilFRA();
	l = arg_w(l);
	for (p = SP; p < (SP + l); p++) {
		check_def(p, l);
		stack_loc(p + l) ^= stack_loc(p);
	}
	st_dec(l);
}

DoXORz()
{
	/* XOR w: Boolean exclusive or on two groups of w bytes */
	register size l = uwpop();
	register ptr p;

	LOG(("@X6 DoXORz(%ld)", l));
	spoilFRA();
	l = arg_w(l);
	for (p = SP; p < (SP + l); p++) {
		check_def(p, l);
		stack_loc(p + l) ^= stack_loc(p);
	}
	st_dec(l);
}

DoCOMl2(arg)
	size arg;
{
	/* COM w: Complement (one's complement of top w bytes) */
	register size l = (L_arg_2() * arg);
	register ptr p;

	LOG(("@X6 DoCOMl2(%ld)", l));
	spoilFRA();
	l = arg_w(l);
	for (p = SP; p < (SP + l); p++) {
		check_def(p, 0);
		stack_loc(p) = ~stack_loc(p);
	}
}

DoCOMz()
{
	/* COM w: Complement (one's complement of top w bytes) */
	register size l = uwpop();
	register ptr p;

	LOG(("@X6 DoCOMz(%ld)", l));
	spoilFRA();
	l = arg_w(l);
	for (p = SP; p < (SP + l); p++) {
		check_def(p, 0);
		stack_loc(p) = ~stack_loc(p);
	}
}

DoROLl2(arg)
	size arg;
{
	/* ROL w: Rotate left a group of w bytes */
	register size l = (L_arg_2() * arg);
	register long s, t = uwpop();
	register long signbit;

	LOG(("@X6 DoROLl2(%ld)", l));
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
#endif	LOGGING
	
	/* calculate result */
	while (t--) {
		s = (s & signbit) ? ((s<<1) | BIT(0)) : (s<<1);
	}
	npush(s, l);
}

DoROLz()
{
	/* ROL w: Rotate left a group of w bytes */
	register size l = uwpop();
	register long s, t = uwpop();
	register long signbit;

	LOG(("@X6 DoROLz(%ld)", l));
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
#endif	LOGGING
	
	/* calculate result */
	while (t--) {
		s = (s & signbit) ? ((s<<1) | BIT(0)) : (s<<1);
	}
	npush(s, l);
}

DoRORl2(arg)
	size arg;
{
	/* ROR w: Rotate right a group of w bytes */
	register size l = (L_arg_2() * arg);
	register long s, t = uwpop();
	register long signbit;

	LOG(("@X6 DoRORl2(%ld)", l));
	spoilFRA();
	signbit = (l == 2) ? SIGNBIT2 : SIGNBIT4;
	s = upop(arg_wi(l));
	
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
#endif	LOGGING
	
	/* calculate result */
	while (t--) {
		/* the >> in C does sign extension, the ROR does not */
		if (s & BIT(0))
			s = (s >> 1) | signbit;
		else	s = (s >> 1) & ~signbit;
	}
	npush(s, l);
}

DoRORz()
{
	/* ROR w: Rotate right a group of w bytes */
	register size l = uwpop();
	register long s, t = uwpop();
	register long signbit;

	LOG(("@X6 DoRORz(%ld)", l));
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
#endif	LOGGING
	
	/* calculate result */
	while (t--) {
		/* the >> in C does sign extension, the ROR does not */
		if (s & BIT(0))
			s = (s >> 1) | signbit;
		else	s = (s >> 1) & ~signbit;
	}
	npush(s, l);
}
