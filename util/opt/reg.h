/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory, 
 *  in the file "Copyright".
 *
 */
#ifndef REG_H_
#define REG_H_

#include "types.h"

void regvar(register arg_p ap);
void outregs(void);
int inreg(offset off);
void outtes(void);
void incregusage(offset off);

#endif /* REG_H_ */
