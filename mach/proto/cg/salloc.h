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

string myalloc(int size);
void myfree(void* p);
void popstr(int nnstab);
char *salloc(int size);
int compar(char **p1, char **p2);
void garbage_collect(void);

#endif /* SALLOC_H_ */
