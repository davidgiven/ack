/* $Id$ */
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

/* Author: E.G. Keizer */

static int level = 0 ;
static wrs() ;
procentry(name) char *name ; {
	register int count ;

	count=level++ ;
	while ( count-- ) {
		wrs("  ") ;
	}
	wrs("Entering ");wrs(name);wrs("\n") ;
}
procexit(name) char *name ; {
	register int count ;

	count= --level ;
	while ( count-- ) {
		wrs("  ") ;
	}
	wrs("Leaving  ");wrs(name);wrs("\n") ;
}
static wrs(s) register char *s ; {
	write(2,s,strlen(s)) ;
}
