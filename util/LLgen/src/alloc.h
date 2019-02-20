/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory, 
 *  in the file "Copyright".
 *
 *  Created on: 2019-02-16
 *  
 */
#ifndef ALLOC_H_
#define ALLOC_H_

#include "types.h"

p_mem alloc(unsigned int size);
p_mem ralloc(p_mem p,unsigned int size);
p_mem new_mem(register p_info p);



#endif /* ALLOC_H_ */
