/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.

 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include    <stdio.h>
#include    <stdlib.h>
#include	"alloc.h"

void No_Mem(void)
{
	fprintf(stderr,"Out of memory\n");
	exit(1);
}
