/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory, 
 *  in the file "Copyright".
 *
 *  Created on: 2019-02-06
 *  
 */
#ifndef EVAL_H_
#define EVAL_H_

#ifndef	LINT


#include <em.h>

struct expr;
struct value;
struct type;

void EVAL(register struct expr *expr, int val, int code, label true_label, label false_label);

/*	assop() generates the opcode of an assignment operators op=	*/
void assop(register struct type *type, int oper);
/*	store_val() generates code for a store operation.
	There are four ways of storing data:
	- into a global variable
	- into an automatic local variable
	- into a local static variable
	- absolute addressing
*/
void store_val(register struct value *vl, register struct type *tp);
void load_val(register struct expr *expr, int rlval);
void load_cst(arith val, arith siz);

#endif /* LINT */

#endif /* EVAL_H_ */
