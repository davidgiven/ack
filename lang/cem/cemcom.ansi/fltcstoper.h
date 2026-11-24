/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory,
 *  in the file "Copyright".
 *
 *  Created on: 2019-02-07
 *
 */
#ifndef FLTCSTOPER_H_
#define FLTCSTOPER_H_

struct expr;

void fltcstbin(struct expr** expp, int oper, struct expr* expr);
void fltcstround(struct expr* expr);

/* Mantissa bits of an 8-byte floating type when the target's floating point
   is not IEEE (-Vm), else 0. */
extern int flt_dbl_bits;

#endif /* FLTCSTOPER_H_ */
