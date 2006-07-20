#ifndef NORCSID
static char rcsid[] = "$Id$";
#endif

#include <stdlib.h>
#include <stdio.h>
#include "param.h"
#include "types.h"
#include "tes.h"
#include "assert.h"
#include "lookup.h"
#include "proinf.h"
#include "optim.h"
#include "ext.h"

/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Hans van Staveren
 */


/* VARARGS1 */
error(s,a) char *s,*a; {

	fprintf(stderr,"%s: error on line %u",progname,linecount);
	if (prodepth != 0)
		fprintf(stderr,"(%.*s)",IDL,curpro.symbol->s_name);
	fprintf(stderr,": ");
	fprintf(stderr,s,a);
	fprintf(stderr,"\n");
#ifndef NDEBUG
	abort();
#endif
	exit(-1);
}

#ifndef NDEBUG
badassertion(file,line) char *file; unsigned line; {

	fprintf(stderr,"assertion failed file %s, line %u\n",file,line);
	error("assertion");
}
#endif

#ifdef DIAGOPT
optim(n) {

	fprintf(stderr,"Made optimization %d",n);
	if (prodepth)
		fprintf(stderr," (%.*s)",IDL,curpro.symbol->s_name);
	fprintf(stderr,"\n");
}
#endif
