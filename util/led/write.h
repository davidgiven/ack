/*
 * write.h
 *
 *  Created on: 2018-11-17
 *      Author: carl
 */

#ifndef __WRITE_H_INCLUDED__
#define __WRITE_H_INCLUDED__

#include "out.h"

void wr_fatal(void);
void begin_write(void);
void end_write(void);
void wrt_emit(char *emit, int sectindex, long cnt);
void wrt_nulls(int sectindex, long cnt);
void wrt_name(struct outname *name, int writename);


#endif /* __WRITE_H_INCLUDED__ */
