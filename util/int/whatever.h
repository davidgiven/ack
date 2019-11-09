/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory,
 *  in the file "Copyright".
 *
 *  Created on: 2019-10-23
 *
 */
#ifndef WHATEVER_H_
#define WHATEVER_H_

#include "global.h"	/* ptr, size */

/*
 * This header is for _whatever_ isn't in another header.
 */

/* disassemble.c */
void disassemble(void);

/* do_misc.c */
void putLIN(long);
void putFIL(ptr);

/* do_proc.c */
void call(long, int);

/* init.c */
void init(int , char **);

/* m_ioctl.c */
int do_ioctl(int, int, ptr);

/* moncalls.c */
void moncall(void);
int memfault(ptr, size);
void efault(int);
void einval(int);

/* tally.c */
void tally(void);
void out_tally(void);

#endif /* WHATEVER_H_ */
