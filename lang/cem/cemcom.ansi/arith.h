/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/* COMPILER ARITHMETIC */

/*	The compiler uses 2 types, arith and writh, for its internal
	arithmetic.  Type arith is normally long, and may be too
	narrow for long long values.  We can't change arith to a wider
	type, because both <em_arith.h> (pulled by <em.h>) and
	<flt_arith.h> define arith.

	Type writh (wide arithmetic) is for values that might not fit
	in arith.  Normally writh is the long long native to the
	source machine, which is always good for local compilations,
	and generally OK too for cross compilations downwards and
	sidewards.
*/
#ifndef ARITH_H_
#define ARITH_H_

#include    <em_arith.h>		/* obtain definition of "arith"	*/

#define	writh		long long
/* The compiler also uses "unsigned writh". */

struct expr;
struct type;

#define	arith_size	(sizeof(arith))
#define	arith_sign	((arith) 1 << (arith_size * 8 - 1))
#define	max_arith	(~arith_sign)

extern writh full_mask[];		/* cstoper.c */

void arithbalance(register struct expr **e1p, int oper, register struct expr **e2p);
void relbalance(register struct expr **e1p, int oper, register struct expr **e2p);
void ch3pointer(struct expr **expp, int oper, register struct type *tp);
int any2arith(register struct expr **expp, register int oper);
void erroneous2int(struct expr **expp);
struct expr *arith2arith(struct type *tp, int oper, register struct expr *expr);
int int2int(struct expr **expp, register struct type *tp);
void int2float(register struct expr **expp, struct type *tp);
void float2int(struct expr **expp, struct type *tp);
void float2float(register struct expr **expp, struct type *tp);
void array2pointer(register struct expr *exp);
void function2pointer(register struct expr *exp);
void string2pointer(register struct expr *ex);
void opnd2integral(register struct expr **expp, int oper);
void opnd2logical(register struct expr **expp, int oper);
void opnd2test(register struct expr **expp, int oper);
void any2opnd(register struct expr **expp, int oper);
void any2parameter(register struct expr **expp);
void field2arith(register struct expr **expp);
void switch_sign_fp(register struct expr *expr);
char *writh2str(writh val, int uns);

#endif /* ARITH_H_ */
