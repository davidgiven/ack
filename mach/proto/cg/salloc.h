/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory, 
 *  in the file "Copyright".
 *
 *  Created on: 2019-04-14
 *  
 */
#ifndef SALLOC_H_
#define SALLOC_H_

#include "types.h"

extern string myalloc(int size);
extern void myfree(void* p);
extern void popstr(int nnstab);
extern char *salloc(int size);
extern int compar(const void* pp1, const void* pp2);
extern void garbage_collect(void);

#endif /* SALLOC_H_ */
