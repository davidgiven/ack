/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Hans van Staveren
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "param.h"
#include "types.h"
#include "tes.h"
#include "lookup.h"
#include "proinf.h"
#include "optim.h"
#include "util.h"
#include "ext.h"

/* VARARGS1 */
void error(char *s, ...)
{
	va_list ap;
	va_start(ap, s);

	fprintf(stderr, "%s: error on line %u", progname, linecount);
	if (prodepth != 0)
		fprintf(stderr, "(%.*s)", IDL, curpro.symbol->s_name);
	fprintf(stderr, ": ");
	vfprintf(stderr, s, ap);
	fprintf(stderr, "\n");

	va_end(ap);
#ifndef NDEBUG
	abort();
#endif
	exit(EXIT_FAILURE);
}

#ifdef DIAGOPT
void optim(int n)
{
	fprintf(stderr,"Made optimization %d",n);
	if (prodepth)
	fprintf(stderr," (%.*s)",IDL,curpro.symbol->s_name);
	fprintf(stderr,"\n");
}
#endif
