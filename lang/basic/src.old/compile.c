/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include "bem.h"

#ifndef NORSCID
static char rcs_id[] = "$Header$" ;
#endif


/* compile the next program in the list */
/* Here we should open the input file. (for the future) */

FILE *yyin;

compileprogram(dummyprog)
char *dummyprog;
{

	while( getline())
		(void) yyparse();
	(void) fclose(yyin);
}
