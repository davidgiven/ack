/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* S H A R E D   F I L E
 *
 * G L O B A L . C 
 */

#include "types.h"

int ps = 0;
int ws = 0;

proc_p		curproc;	/* current procedure */

char		*filename;	/* name of current input file */

lset		mesregs;	/* set of MES ms_reg pseudos */

short		time_space_ratio = 50;
				/* 0   if optimizing for space only,
				 * 100 if optimizing for time only,
				 * else something 'in between'.
				 */
