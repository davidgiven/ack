/*  Copyright (c) 2019 ACK Project. See the file Copyright in
 *  the project root directory for more information.
 *
 *  Created on: 2019-02-06
 *
 */
#ifndef CH3_H_
#define CH3_H_

/* Structure forward declarations. */
struct expr;
struct type;
struct proto;
struct idf;


void ch3sel(struct expr **expp, int oper, struct idf *idf);
void ch3incr(struct expr **expp, int oper);
void ch3cast(register struct expr **expp, int oper, register struct type *tp);
int equal_type(register struct type *tp,register struct type *otp, int qual_lev, int diag);
int check_pseudoproto(register struct proto *pl,register struct proto *opl, int diag);
int legal_mixture(struct type *tp, struct type *otp, int diag);
int equal_proto(register struct proto *pl, register struct proto *opl, int diag);
int recurqual(struct type *tp, int qual);
void ch3asgn(struct expr **expp, int oper, struct expr *expr);
int is_integral_type(register struct type *tp);
int is_arith_type(register struct type *tp);

#endif /* CH3_H_ */
