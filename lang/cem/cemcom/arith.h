/* $Header$ */
/* COMPILER ARITHMETIC */

/*	Normally the compiler does its internal arithmetics in longs
	native to the source machine, which is always good for local
	compilations, and generally OK too for cross compilations
	downwards and sidewards.  For upwards cross compilation and
	to save storage on small machines, SPECIAL_ARITHMETICS will
	be handy.
*/

#include	"spec_arith.h"

#ifndef	SPECIAL_ARITHMETICS

#define	arith	long				/* native */

#else	SPECIAL_ARITHMETICS

/* not implemented yet */
#define	arith	int				/* dummy */

#endif	SPECIAL_ARITHMETICS
