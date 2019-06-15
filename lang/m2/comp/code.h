/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory, 
 *  in the file "Copyright".
 *
 *  Created on: 2019-02-26
 *  
 */
#ifndef CODE_H_
#define CODE_H_

#include "parameters.h"
#include <em_arith.h>
#include <em_label.h>

/* Forward declarations. */
struct node;
struct desig;
struct type;


/*	Generate code to push constant "cst" with size "size" bytes. */
void CodeConst(arith cst, int size);
/* Generate constant character or string. */
void CodeString(register struct node *nd);
/* Generate code for an expression contained in "nd". */
void CodeExpr(struct node *nd, struct desig *ds, label true_label, label false_label);
/* Generate implicit type conversion code. */
void CodeCoercion(struct type *t1, struct type *t2);
/*	Generate code for a procedure call including parameter setup. Checking of parameters
	and result is already done. */
void CodeCall(struct node *nd);
void CodePString(struct node *nd, struct type *tp);
/* Generate a range check if necessary */
void RangeCheck(register struct type *tpl, struct type *tpr);
void CodeOper(register struct node *expr, label true_label, label false_label);
/*	Generate code to push the value of the expression "nd"
	on the stack. */
void CodePExpr(register struct node *nd);
/*	Generate code to store the expression on the stack into the
	designator "nd".
*/
void CodeDStore(register struct node *nd);

#ifdef SQUEEZE
void c_bra(label l);
void c_loc(int n);
void c_lae_dlb(label l);
void CAL(char *name, int ssp);
#endif




#endif /* CODE_H_ */
