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
void dt_stdp(ptr addr, ptr ap);
void dt_stn(ptr addr, long al, size n);
void dt_stw(ptr addr, long al);
void dt_stip(ptr addr, ptr ap);
#ifndef NOFLOAT
void dt_stf(ptr addr, double f, size n);
#endif

ptr dt_ldip(ptr addr);
ptr dt_lddp(ptr addr);
unsigned long dt_ldu(ptr addr, size n);
unsigned long dt_lduw(ptr addr);
long dt_lds(ptr addr, size n);
long dt_ldsw(ptr addr);
void dt_mvd(ptr d2, ptr d1, size n);
void dt_mvs(ptr d, ptr s, size n);

#endif /* DATA_H_ */
