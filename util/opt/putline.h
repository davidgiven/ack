/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory, 
 *  in the file "Copyright".
 *
 *  Created on: 2019-04-06
 *  
 */
#ifndef PUTLINE_H_
#define PUTLINE_H_

#include "types.h"

void putlines(register line_p lnp);
void outdef(register sym_p sp);
void outocc(register sym_p sp);
void outpro(void);
void outend(void);
void outinst(int m);
void outoff(offset off);
void outint(short i);
void outshort(short i);
void outnum(register num_p np);
void outsym(register sym_p sp);

#endif /* PUTLINE_H_ */
