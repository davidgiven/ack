#include "bem.h"

#ifndef NORSCID
static char rcs_id[] = "$Header$" ;
#endif

int	listing;		/* -l listing required */
int	debug;			/* -d compiler debugging */
int	wflag=1;		/* -w no warnings */
int	hflag=0;		/* -h<number> to split EM program */
int	traceflag=0;		/* generate line tracing code */
int	nolins=0;		/* generate no LIN statements */

parseparams(argc,argv)
int argc;
char **argv;
{
	int i,j,k;
	char *ext;

	j=k=0;
	if(argc< 4)
	{
	fprintf(stderr,"usage %s <flags> <file>.i <file>.e <source>\n", argv[0]);
	exit(-1);
	}
	for(i=1;i<argc;i++)
	if( argv[i][0]=='-')
		switch(argv[i][1])
		{
		case 'D': yydebug++; break;	/* parser debugging */
		case 't': traceflag++; break;	/* line tracing */
		case 'h':/* split EM file */
			hflag=0;
			threshold= (long) atol(argv[i][2]);
			if( threshold==0)
				threshold= THRESHOLD;	
			break;
		case 'd': debug++; break;
		case 'l': nolins++; break;	/* no EM lin statements */
		case 'E': listing++; break;	/* generate full listing */
		} else {
			/* new input file */
			ext= argv[i]+strlen(argv[i])-1;
			if( *(ext-1) != '.')
				/* should be the source file name */
				program= argv[i];
			else
			if( *ext == 'i')
				inpfile= argv[i];
		 	else
			if( *ext == 'e')
				outfile= argv[i];
		}
}
