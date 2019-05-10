/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory, 
 *  in the file "Copyright".
 *
 *  Created on: 2019-05-03
 *  
 */
#ifndef COERC_H_
#define COERC_H_

#include "iocc.h"

struct varinfo;


void n_move(int s1, int e1, int s2, int e2, struct varinfo *vi);
int existalmove(iocc_t from, int prpno);
struct varinfo *gen_move(iocc_t from, iocc_t to);
void n_test(int s, int e, struct varinfo *vi);
struct varinfo *gen_test(iocc_t from);
struct varinfo *gen_label(int arg);
struct varinfo *gen_preturn(void);
struct varinfo *gen_tlab(int n);
void n_stack(int s, int e, int p, struct varinfo *vi);
void checkstacking(register short *sp);
/* cgg.y is buggy!!! it has one less parameter.. */
/*void n_coerc(int ti, int be, struct varinfo *al, struct varinfo *ge, struct varinfo *rp, iocc_t in);*/
void checkunstacking(int setno);

#endif /* COERC_H_ */
