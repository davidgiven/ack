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

/*
 *  L L G E N
 *
 *  An Extended LL(1) Parser Generator
 *
 *  Author : Ceriel J.H. Jacobs
 */

/*
 * alloc.c
 * Interface to malloc() and realloc()
 */

# include "types.h"
# include "extern.h"

static string rcsid = "$Header$";

static string e_nomem = "Out of memory";

p_mem
alloc(size) unsigned size; {
	register p_mem	p;
	p_mem		malloc();

	if ((p = malloc(size)) == 0) fatal(linecount,e_nomem);
	return p;
}

p_mem
ralloc(p,size) p_mem p; unsigned size; {
	register p_mem	q;
	p_mem		realloc();

	if ((q = realloc(p,size)) == 0) fatal(linecount,e_nomem);
	return q;
}
