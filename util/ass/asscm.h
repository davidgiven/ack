/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory, 
 *  in the file "Copyright".
 *
 *  Created on: 2019-03-12
 *  
 */
#ifndef ASSCM_H_
#define ASSCM_H_

/* Allocates an area of "size" bytes in memory
 * and returns a pointer to this area.
 */
area_t getarea(unsigned int size);
/* Frees an area of memory of "size" bytes. */
void freearea(register area_t ptr, unsigned int size);

#ifdef MEMUSE
void memuse(void);
#endif

#endif /* ASSCM_H_ */
