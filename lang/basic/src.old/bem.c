#include "bem.h"

#ifndef NORSCID
static char rcs_id[]	= "$Header$" ;
static char rcs_bem[]	= RCS_BEM ;
static char rcs_symb[]	= RCS_SYMB ;
static char rcs_graph[]	= RCS_GRAPH ;
#endif

/* Author: M.L. Kersten
**
** This is the main routine for the BASIC-EM frontend.
** Program parameters are decoded, the BASIC program is parsed
** and compiled to an executable program
**
** Bem expects at least three parameters. One ending with '.i' is considered
** the input to the compiler, '.e' denotes the file to be generated,
** and the last name denotes the name of the user supplied file name.
** The latter is used to store the data entries.
** Additional flags may be supplied, see parseparms.
*/

char	*program;

char	datfname[MAXFILENAME] ;
char	tmpfname[MAXFILENAME] ;

char	*inpfile, *outfile;
main(argc,argv)
int argc;
char **argv;
{
	extern int errorcnt;

	/* parseparams */
	parseparams(argc,argv);
	/* initialize the system */
	initialize();
	/* compile source programs */
	compileprogram(program);
	linewarnings();
	if( errorcnt) exit(-1);
	/* process em object files */
	simpleprogram();
}
