/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include <stdio.h>
char rcs_id[] = "$Id$" ;

typedef struct {
	int	(*p_func)() ;
	char	*p_slink ;
} p_fiptr ;

#ifndef NOFLOAT
typedef struct {
	double	(*p_func)() ;
	char	*p_slink ;
} p_ffptr ;
#endif

int kwad(val) int val ; { return val*val ; }
cmain() {
	p_fiptr p_kwad ;

	/* Test calling pascal procedures */
	if ( twice(7)!=14 || twice(-9)!=-18 ) {
		printf("Calling pascal from C doesn't work\n") ;
		fflush(stdout) ;
		}
	else
		incs() ;
	/* Test passing C function pointers */
	p_kwad.p_slink= (char *)0 ; p_kwad.p_func= kwad ;
	cptr(p_kwad) ;
}
pptr(p_twice) p_fiptr p_twice ; {
	if ( p_twice.p_slink!=(char *)0 ) {
		printf("Pascal outer procedure static link unequal to zero\n") ;
		fflush(stdout) ;
		}
	
	if ( p_twice.p_func(-7)!=-14 || p_twice.p_func(9)!=18 ) {
		printf("Passing pascal functions to C doesn't work\n") ;
		fflush(stdout) ;
		}
	else	incs() ;
}

#ifndef NOFLOAT
double callpas(pasfunc,par) p_ffptr pasfunc ; int par ; {
	/* Call a Pascal function, both inner block and outer block */
	/* Function must return a double, (In pascal terms: real) */
	/* and have one integer parameter */
	/* The static link - if present - must be the first parameter */
	if ( pasfunc.p_slink ) {
		return (*pasfunc.p_func)(pasfunc.p_slink,par) ;
	} else {
		return (*pasfunc.p_func)(par) ;
	}
}

int ceval(p_inside) p_ffptr p_inside ; {
	double resval ;
	resval= callpas(p_inside,2) ;
	return resval>1.41 && resval<1.42 ;
}
#endif
