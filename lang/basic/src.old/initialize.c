#include "bem.h"
#include <em_path.h>

#ifndef NORSCID
static char rcs_id[] = "$Header$" ;
#endif

/* generate temporary files etc */

FILE	*emfile;
FILE	*tmpfile;
FILE	*datfile;

initialize()
{
	register char *cindex, *cptr ;

	sprintf(tmpfname,"%s/abc%d",TMP_DIR,getpid());
	/* Find the basename */
	/* Strip leading directories */
	cindex= (char *)0 ;
	for ( cptr=program ; *cptr ; cptr++ ) if ( *cptr=='/' ) cindex=cptr ;
	if ( !cindex ) cindex= program ;
	else {
		cindex++ ;
		if ( !*cindex ) {
			warning("Null program name, assuming \"basic\"") ;
			cindex= "basic" ;
		}
	}
	cptr=datfname ;
	while ( *cptr++ = *cindex++ ) ;
	/* Strip trailing suffix */
	if ( cptr>datfname+3 && cptr[-3]=='.' ) cptr[-3]=0 ;
	strcat(datfname,".d");
	yyin= fopen(inpfile,"r");
	emfile= fopen(outfile,"w");
	tmpfile= fopen(tmpfname,"w");
	if( yyin==NULL || emfile== NULL || tmpfile== NULL )
		fatal("Improper file permissions");
	fillkex();	/* initialize symbol table */
	fprintf(emfile,"#\n");
	fprintf(emfile," mes 2,EM_WSIZE,EM_PSIZE\n");
	initdeftype();	/* set default symbol declarers */
}
