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
