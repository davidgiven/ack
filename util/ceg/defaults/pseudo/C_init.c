#include <em.h>
#include <system.h>
#include "mach.h"

C_init( wsize, psize)
arith wsize, psize;
{
	if ( wsize != EM_WSIZE) {
		fprint( STDERR, "wrong word size\n");
		exit( -1);
	}
	if ( psize != EM_PSIZE) {
		fprint( STDERR, "wrong pointer size\n");
		exit( -1);
	}

	init_back();
}
