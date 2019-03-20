/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory, 
 *  in the file "Copyright".
 *
 *  Created on: 2019-03-16
 *  
 */
#ifndef SEGMENT_H_
#define SEGMENT_H_

#include "global.h"

void init_AB_list(void);
void push_frame(ptr);
void pop_frames(void);
int ptr2seg(ptr);

#endif /* SEGMENT_H_ */
