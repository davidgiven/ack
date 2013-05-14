/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

#include "bem.h"

#ifndef NORSCID
static char rcs_id[] = "$Id$" ;
#endif


/* compile the next program in the list */
/* Here we should open the input file. (for the future) */

File *yyin;

compileprogram()
{
	extern int basicline;

	prologcode();
	prolog2(); /* Some statements are moved from prolog2 to 
                      epilogcode in the new version of the compiler */

	while( basicline = 0, getinputline())
		(void) LLparse();
	epilogcode(); 	
	sys_close(yyin);
}
