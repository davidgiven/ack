/*
 * (c) copyright 1983 by the Vrije Universiteit, Amsterdam, The Netherlands.
 *
 *          This product is part of the Amsterdam Compiler Kit.
 *
 * Permission to use, sell, duplicate or disclose this software must be
 * obtained in writing. Requests for such permissions may be sent to
 *
 *      Dr. Andrew S. Tanenbaum
 *      Wiskundig Seminarium
 *      Vrije Universiteit
 *      Postbox 7161
 *      1007 MC Amsterdam
 *      The Netherlands
 *
 */

char rcs_id[] = "$Header$" ;

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
