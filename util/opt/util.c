#include <stdio.h>
#include "param.h"
#include "types.h"
#include "assert.h"
#include "lookup.h"
#include "proinf.h"
#include "optim.h"
#include "ext.h"

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
	abort();
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
	if (inpro)
		fprintf(stderr," (%.*s)",IDL,curpro.symbol->s_name);
	fprintf(stderr,"\n");
}
#endif
