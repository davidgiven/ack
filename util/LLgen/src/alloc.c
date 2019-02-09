/* Copyright (c) 1991 by the Vrije Universiteit, Amsterdam, the Netherlands.
 * For full copyright and restrictions on use see the file COPYING in the top
 * level of the LLgen tree.
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

#include <stdlib.h>
#include <stdio.h>
# include "types.h"
# include "extern.h"

# ifndef NORCSID
static string rcsid = "$Id$";
# endif

static string e_nomem = "Out of memory";

/*
   Allocate "size" bytes. Panic if it fails
 */
p_mem alloc(unsigned int size)
{
	p_mem	p;

	if ((p = malloc(size)) == 0) fatal(linecount,e_nomem, NULL);
	return p;
}

/*
   Re-allocate the chunk of memory indicated by "p", to
   occupy "size" bytes
 */
p_mem ralloc(p_mem p,unsigned int size)
{
	if ((p = realloc(p,size)) == 0) fatal(linecount,e_nomem, NULL);
	return p;
}

/*
   This routine implements arrays that can grow.
   It must be called every time a new element is added to it.
   Also, the array has associated with it a "info_alloc" structure,
   which contains info on the element size, total allocated size,
   a pointer to the array, a pointer to the first free element,
   and a pointer to the top.
   If the base of the array is remembered elsewhere, it should
   be updated each time this routine is called
 */
p_mem new_mem(register p_info p)
{
	p_mem	rp;
	unsigned sz;

	if (p->i_max >= p->i_top) {	/* No more free elements */
		sz = p->i_size;
		if (sizeof(char *) > 2) {
			/*
			   Do not worry about size. Just double it.
			 */
			p->i_size += p->i_size;
			if (! p->i_size)
				p->i_size += p->i_incr * p->i_esize;
			}
		else {
			/*
			   Worry about size, so only increment in chunks of i_incr.
			 */
			p->i_size += p->i_incr * p->i_esize;
		}
		p->i_ptr = !p->i_ptr ?
			alloc(p->i_size) :
			ralloc(p->i_ptr, p->i_size);
		p->i_max = p->i_ptr + sz;
		p->i_top = p->i_ptr + p->i_size;
	}
	rp = p->i_max;
	p->i_max += p->i_esize;
	return rp;
}
