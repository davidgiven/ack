#include "bem.h"

/* compile the next program in the list */

FILE *yyin;

compileprogram()
{

	while( getline())
		yyparse();
	fclose(yyin);
}
