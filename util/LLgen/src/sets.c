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
 * sets.c
 * Set manipulation and allocation routines.
 */

#include  <stdio.h>
# include <assert.h>
# include "types.h"
# include "extern.h"
# include "sets.h"

# ifndef NORCSID
static string rcsid9 = "$Id$";
# endif



int		nbytes;
static int	setsize;
int		tsetsize;
p_set		*setptr, *maxptr;
static t_info	set_info;
p_mem		alloc();

/*
 * Initialises some variables needed for setcomputations
 */
void setinit(int nt_needed)
{
	register int	 bitset;

	nbytes = NBYTES(ntokens);
	bitset = ALIGN(nbytes);
	tsetsize = NINTS(bitset);
	if (nt_needed) {
		/* nonterminals must be included in the sets */
		bitset += NBYTES(nnonterms);
	}
	setsize = NINTS(bitset);
	set_info.i_esize = sizeof(p_set);
	set_info.i_incr = 20;
}

p_set get_set(void)
{
	/*
	 * Allocate a set that cannot be freed
	 */
	register p_set p, q;
	static p_set sets, maxsets;

	if ((p = sets) >= maxsets) {
		q = p = (p_set) alloc((unsigned) (50*setsize*sizeof(*sets)));
		maxsets = p + 50 * setsize;
		do {
			*q++ = 0;
		} while (q < maxsets);
	}
	sets = p + setsize;;
	return p;
}

p_set setalloc(void)
{
	/*
	 * Allocate a set which can later be freed.
	 */
	register p_set	p;
	register int	size = setsize;

	p = (p_set) alloc((unsigned) (size * sizeof(*p))) + size;
	do {
		*--p = 0;
	} while (--size);
	return p;
}

int setunion(register p_set a,register p_set b)
{
	/*
	 * a = a union b.
	 * Return 1 if the set a changed
	 */
	register int	i;
	register int	j;
	register int	nsub = 0;

	i = setsize;
	do {
		*a = (j = *a) | *b++;
		if (*a++ != j) {
			nsub = 1;
		}
	} while (--i);
	return nsub;
}

int setintersect(register p_set a,register p_set b)
{
	/*
	 * a = a intersect b.
	 * return 1 if the result is empty
	 */
	register int	i;
	register int	nempty;

	nempty = 1;
	i =  setsize;
	do {
		if (*a++ &= *b++) nempty = 0;
	} while (--i);
	return nempty;
}

void setminus(register p_set a,register p_set b)
{
	/*
	 * a = a setminus b
	 */
	register int	i;

	i = setsize;
	do {
		*a++ &= ~(*b++);
	} while (--i);
}

int setempty(register p_set p)
{
	/*
	 * Return 1 if the set p is empty
	 */
	register int	i;

	i = tsetsize;
	do {
		if (*p++) return 0;
	} while (--i);
	return 1;
}

int findindex(p_set set)
{
	/*
	 * The set "set" will serve as a recovery set.
	 * Search for it in the table. If not present, enter it.
	 * Here is room for improvement. At the moment, the list of
	 * sets is examined with linear search.
	 */
	register p_set	*t;
	p_mem		new_mem();
	register p_set	a;
	register p_set	b;
	register int	i;
	int		saved;

	/*
	 * First search for the set in the table
	 */
	for (t = setptr; t < maxptr; t++) {
		a = *t;
		b = set;
		i = tsetsize;
		do {
			if (*a++ != *b++) break;
		} while (--i);
		if (i) continue;
		/*
		 * Here, the sets are equal.
		 */
		return nbytes * (t - setptr);
	}
	/*
	 * Now check if the set consists of only one element.
	 * It would be a waste to use a set for that
	 */
	if (setcount(set, &saved) == 1) return -(saved + 1);
	/*
	 * If it does, return its number as a negative number.
	 */
	maxptr = (p_set *) new_mem(&set_info);
	setptr = (p_set *) set_info.i_ptr;
	*maxptr = setalloc();
	setunion(*maxptr, set);
	return nbytes * (maxptr++ - setptr);
}

int setcount(register p_set set, int *saved)
{
	register int i, j;

	for (j = 0, i = 0; i < ntokens; i++) {
		if (IN(set,i)) {
			j++;
			*saved = i;
		}
	}
	return j;
}
