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

/* generate temporary files etc */

File	*datfile;

extern void fillkex(void);

void initialize(void)
{
	register char *cindex, *cptr;
        int result1, result2;

	/* Find the basename */
	/* Strip leading directories */
	cindex= (char *)0;
	for ( cptr=program; *cptr; cptr++ ) if ( *cptr=='/' ) cindex=cptr;
	if ( !cindex ) cindex= program;
	else {
		cindex++;
		if ( !*cindex ) {
			warning("Null program name, assuming \"basic\"");
			cindex= "basic";
		}
	}
	cptr=datfname;
	while ( (*cptr++ = *cindex++) !=0 );
	/* Strip trailing suffix */
	if ( cptr>datfname+3 && cptr[-3]=='.' ) cptr[-3]=0;
	strcat(datfname,".d");
	C_init((arith)BEMINTSIZE, (arith)BEMPTRSIZE);
	result1 = sys_open(inpfile, OP_READ, &yyin);
	result2 = C_open(outfile);
	if ( result1==0 || result2== 0 )
		fatal("Improper file permissions");
	C_magic();
	fillkex();	/* initialize symbol table */
	C_ms_emx((arith)BEMINTSIZE,(arith)BEMPTRSIZE);
	initdeftype();	/* set default symbol declarers */
}
