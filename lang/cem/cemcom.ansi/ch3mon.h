/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory,
 *  in the file "Copyright".
 *
 *  Created on: 2019-02-06
 *
 */
#ifndef CH3MON_H_
#define CH3MON_H_

struct expr;

void ch3mon(int oper, struct expr** expp);
void ch3bin(struct expr** expp, int oper, struct expr* expr);
void pntminuspnt(struct expr** expp, int oper, struct expr* expr);
int arg_switched(int oper);
void mk_binop(struct expr** expp, int oper, struct expr* expr, int commutative);
void pointer_arithmetic(struct expr** expp1, int oper, struct expr** expp2);
void pointer_binary(struct expr** expp, int oper, struct expr* expr);

#endif /* CH3MON_H_ */
