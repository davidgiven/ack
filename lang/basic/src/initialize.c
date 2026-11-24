/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

#include <stdio.h>
#include "bem.h"
#include "util.h"

#ifndef NORCSID
static char rcs_id[] = "$Id$";
#endif

extern void fillkex(void);

void initialize(void)
{
        int result;

	C_init((arith)BEMINTSIZE, (arith)BEMPTRSIZE);
	yyin = fopen(inpfile, "r");
	result = C_open(outfile);
	if ( !yyin || result== 0 )
		fatal("Improper file permissions");
	C_magic();
	fillkex();	/* initialize symbol table */
	C_ms_emx((arith)BEMINTSIZE,(arith)BEMPTRSIZE);
	initdeftype();	/* set default symbol declarers */
}
