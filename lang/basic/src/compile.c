/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

/*#include "bem.h"*/
#include <stdio.h>
#include "system.h"
#include "gencode.h"
#include "bem.h"

#ifndef NORCSID
static char rcs_id[] = "$Id$" ;
#endif

extern int getinputline(void);
extern void LLparse(void);

/* compile the next program in the list */
/* Here we should open the input file. (for the future) */

File *yyin;

void compileprogram(void)
{
	extern int basicline;

	prologcode();
	prolog2(); /* Some statements are moved from prolog2 to 
                      epilogcode in the new version of the compiler */

	while( basicline = 0, getinputline())
	{
		LLparse();
	}
	epilogcode(); 	
	sys_close(yyin);
}
