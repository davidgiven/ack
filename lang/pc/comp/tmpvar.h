/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory,
 *  in the file "Copyright".
 *
 */
#ifndef TMPVAR_H_
#define TMPVAR_H_

#include "em_arith.h"


struct scope;

void TmpOpen(struct scope *sc);
arith TmpSpace(arith sz, int al);
arith NewInt(int reg_prior);
arith NewPtr(int reg_prior);
void FreeInt(arith off);
void FreePtr(arith off);
void TmpClose(void);


#endif /* TMPVAR_H_ */
