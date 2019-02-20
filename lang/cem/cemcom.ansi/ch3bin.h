/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory,
 *  in the file "Copyright".
 *
 *  Created on: 2019-02-06
 *
 */
#ifndef CH3BIN_H_
#define CH3BIN_H_

struct expr;

void ch3bin(register struct expr **expp, int oper, struct expr *expr);
void pntminuspnt(register struct expr **expp, int oper, register struct expr *expr);
int arg_switched(int oper);
void mk_binop(struct expr **expp, int oper, register struct expr *expr, int commutative);
void pointer_arithmetic(register struct expr **expp1, int oper, register struct expr **expp2);
void pointer_binary(register struct expr **expp, int oper, register struct expr *expr);


#endif /* CH3BIN_H_ */
