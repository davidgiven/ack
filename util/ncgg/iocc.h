/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
#ifndef IOCC_H_
#define IOCC_H_

#include "param.h"

typedef struct iocc {
	short in_set[SETSIZE];
	int   in_index;
} iocc_t,*iocc_p;

extern iocc_t subr_iocc(int tokarg, int subreg);
extern iocc_t tokm_iocc(int tokarg, char *ident);
extern iocc_t percident_iocc(char *ident);
extern iocc_t ident_iocc(char *ident);
extern iocc_t all_iocc(int all_no, int subreg);
extern iocc_t descr_iocc(char *ident);

#endif
