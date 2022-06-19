/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory, 
 *  in the file "Copyright".
 *
 */
#ifndef DATA_H_
#define DATA_H_

#include "global.h"


void init_data(ptr hb);
void newHP(ptr ap);
void dt_stdp(register ptr addr, ptr ap);
void dt_stn(register ptr addr, long al, size n);
void dt_stw(register ptr addr, long al);
void dt_stip(register ptr addr, ptr ap);
#ifndef	NOFLOAT
void dt_stf(register ptr addr, double f, register size n);
#endif

ptr dt_ldip(ptr addr);
ptr dt_lddp(register ptr addr);
unsigned long dt_ldu(register ptr addr, size n);
unsigned long dt_lduw(register ptr addr);
long dt_lds(register ptr addr, size n);
long dt_ldsw(register ptr addr);
void dt_mvd(ptr d2, ptr d1, size n);
void dt_mvs(ptr d, ptr s, size n);

#endif /* DATA_H_ */
