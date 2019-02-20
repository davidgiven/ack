/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory,
 *  in the file "Copyright".
 *
 *  Created on: 2019-02-06
 *
 */
#ifndef BLOCKS_H_
#define BLOCKS_H_

#include "parameters.h"
#include "arith.h"

int suitable_sz(arith sz, int al);
void store_block(arith sz, int al);
void load_block(arith sz, int al);
void copy_block(arith sz, int al);
void copy_loop(arith sz, arith src, arith dst);

#endif /* BLOCKS_H_ */
