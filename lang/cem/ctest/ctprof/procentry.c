/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

char rcs_id[] = "$Id$" ;

static int level = 0 ;
int procentry(name) char *name ; {
	register int count ;

	count=level++ ;
	while ( count-- ) {
		printf("  ") ;
	}
	printf("Entering %s\n",name) ;
}
int procexit(name) char *name ; {
	register int count ;

	count= --level ;
	while ( count-- ) {
		printf("  ") ;
	}
	printf("Leaving  %s\n",name) ;
}
