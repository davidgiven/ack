#include "bem.h"

#ifndef NORSCID
static char rcs_id[] = "$Header$" ;
#endif


/* compile the next program in the list */

FILE *yyin;

compileprogram()
{

	while( getline())
		yyparse();
	fclose(yyin);
}
