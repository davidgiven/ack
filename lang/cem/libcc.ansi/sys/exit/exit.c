/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "atexits.h"

void (*__functab[NEXITS])(void);
int __funccnt = 0;

void exit(int status)
{
	/* "Called in reversed order of their registration" */
	while (__funccnt >= 0)
		(*__functab[__funccnt])();

	_exit(status);
}
