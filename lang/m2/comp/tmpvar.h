/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory, 
 *  in the file "Copyright".
 *
 *  Created on: 2019-02-27
 *  
 */
/* T E M P O R A R Y   V A R I A B L E S */
#ifndef TMPVAR_H_
#define TMPVAR_H_

#include "em_arith.h"

struct scope;

void TmpOpen(struct scope *sc);
arith TmpSpace(arith sz, int al);
arith NewInt(void);
arith NewPtr(void);
void FreeInt(arith off);
void FreePtr(arith off);
void TmpClose(void);



#endif /* TMPVAR_H_ */
