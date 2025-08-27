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

#include <em_arith.h> /* obtain definition of "arith"	*/
#include <em_label.h> /* obtain definition of "label" */

#define writh long long
/* The compiler also uses "unsigned writh". */

struct expr;
struct type;

#define arith_size (sizeof(arith))
#define arith_sign ((arith)1 << (arith_size * 8 - 1))
#define max_arith  (~arith_sign)

extern writh full_mask[]; /* cstoper.c */

void arithbalance(struct expr** e1p, int oper, struct expr** e2p);
void relbalance(struct expr** e1p, int oper, struct expr** e2p);
void ch3pointer(struct expr** expp, int oper, struct type* tp);
int any2arith(struct expr** expp, int oper);
void erroneous2int(struct expr** expp);
struct expr* arith2arith(struct type* tp, int oper, struct expr* expr);
int int2int(struct expr** expp, struct type* tp);
void int2float(struct expr** expp, struct type* tp);
void float2int(struct expr** expp, struct type* tp);
void float2float(struct expr** expp, struct type* tp);
void array2pointer(struct expr* exp);
void function2pointer(struct expr* exp);
void string2pointer(struct expr* ex);
void opnd2integral(struct expr** expp, int oper);
void opnd2logical(struct expr** expp, int oper);
void opnd2test(struct expr** expp, int oper);
void any2opnd(struct expr** expp, int oper);
void any2parameter(struct expr** expp);
void field2arith(struct expr** expp);
void switch_sign_fp(struct expr* expr);
char* writh2str(writh val, int uns);
label code_string(char* val, int len);

#endif /* ARITH_H_ */
