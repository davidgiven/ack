/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include <stdio.h>
#include <stdlib.h>
#include "system.h"

void C_internal_error(void)
{
	fputs("internal error\n", stderr);
	exit(1);
}
