/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

char rcs_id2[] = "$Id$" ;

/* Author: E.G. Keizer */

int fac(n) { return ( n==0 ? 1 : n*fac(n-1)) ; }

main() {
	{ int dummy ;
		if ( fac(6) != 720 ) printf("vernielt return waarde\n") ;
	}
	return 0 ;
}
