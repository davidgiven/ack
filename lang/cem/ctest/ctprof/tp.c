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

char rcs_id2[] = "$Header$" ;

/* Author: E.G. Keizer */

int fac(n) { return ( n==0 ? 1 : n*fac(n-1)) ; }

main() {
	{ int dummy ;
		if ( fac(6) != 720 ) printf("vernielt return waarde\n") ;
	}
	return 0 ;
}
