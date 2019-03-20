/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory, 
 *  in the file "Copyright".
 *
 *  Created on: 2019-03-15
 *  
 */
#ifndef STACK_H_
#define STACK_H_

#include "global.h"


void init_stack(void);
void newSP(ptr ap);
void incSP(size n);
void decSP(size n);
void newLB(ptr p);
void st_stdp(register ptr addr, ptr ap);
void st_stip(register ptr addr, ptr ap);
void st_stn(register ptr addr, long al, size n);
void st_stw(register ptr addr, long al);
void st_stf(register ptr addr, double f, size n);
ptr st_lddp(register ptr addr);
ptr st_ldip(register ptr addr);
unsigned long st_ldu(register ptr addr, size n);
unsigned long st_lduw(register ptr addr);
long st_lds(register ptr addr, size n);
long st_ldsw(register ptr addr);
double st_ldf(register ptr addr, size n);
void st_mvs(register ptr s2, register ptr s1, size n);
void st_mvd(ptr s, ptr d, size n);
ptr dppop(void);
unsigned long upop(size n);
unsigned long uwpop(void);
long spop(size n);
long swpop(void);
void pop_dt(ptr d, size n);
void popw_dt(ptr d);
void pop_st(ptr s, size n);
void popw_st(ptr s);
double fpop(size n);
long wpop(void);
void dppush(ptr p);
void wpush(long l);
void npush(register long l, register size n);
void push_dt(ptr d, size n);
void pushw_dt(ptr d);
void push_st(ptr s, size n);
void pushw_st(ptr s);
void fpush(double f, size n);

#endif /* STACK_H_ */
