#include "bem.h"

#ifndef NORSCID
static char rcs_id[] = "$Header$" ;
#endif

/* Author: M.L. Kersten
** yywrap is called upon encountering endoffile on yyin.
** when more input files are present, it moves to the next
** otherwise -1 is returned and simultaneous endofinput is set
*/
int endofinput =0;


yywrap()
{
	if( fclose(yyin) == EOF)
		fatal("fclose problems ");
	/* check for next input file */
	return(-1);
}
